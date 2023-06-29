/*
Base source from imgui\examples\example_win32_directx9\main.cpp
*/

#include <cstdlib>
#include <d3d9.h>
#include <tchar.h>
#include <Windows.h>

#include "ssh.h"
#include "shell.h"
#include "explorer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"



// Data
static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

SSH ssh;
static int CurrentIdx = -1;
static int cnt = 0;
static int LogedIn = false;

#ifdef _DEBUG
int main(int argc, char* argv[])
#else
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
#endif
{
	// Create application window
	// ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"NebularSurfer", NULL };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"NebularSurfer", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImVector<ImWchar> ranges;
	ImFontGlyphRangesBuilder builder;

	io.Fonts->AddFontFromFileTTF("fonts/NotoSansKR-Medium.otf", 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());
	//io.Fonts->AddFontFromFileTTF("fonts/NotoSansJP-Medium.otf", 16.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//io.Fonts->AddFontFromFileTTF("fonts/NotoSansTC-Medium.otf", 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	//io.Fonts->Build();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ssh.setCnfList();

	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (ImGui::BeginMainMenuBar()) {

			if (ImGui::BeginMenu("New server explorer")) {
				1 + 1;

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("New local explorer")) {
				1 + 1;

				ImGui::EndMenu();
			}

			ImGui::Separator();

			ImGui::EndMainMenuBar();
		}

		Explorer::Local::Render("Explorer");

		if(!LogedIn) {
			ImGui::Begin("Welcome");
			ImGui::Text("A open-source free ssh client for Windows.\nYou can review the codes on https://github.com/roy6307/NebulaSurfer");
			ImGui::End();
		}
		//ImU32
		//ImColor
		if(!LogedIn) {
			static int RadioVal = 0;

			ImGui::Begin("Main panel");
			ImGui::Text("SSH config");
			ImGui::InputText("Host address(ipv4)", &ssh.host);
			ImGui::InputInt("Port", &ssh.port);
			ImGui::InputText("Username", &ssh.username);
			ImGui::InputText("Password", &ssh.password);
			if(RadioVal == 1) ImGui::InputText("Path to pub key", &ssh.pathToPubKey);
			if (RadioVal == 1) ImGui::InputText("Path to private Key", &ssh.pathToPrivKey);

			ImGui::Text("Login option");

			ImGui::SameLine();  ImGui::RadioButton("Use password", &RadioVal, 0);
			ImGui::SameLine();  ImGui::RadioButton("Use *.pub and *.key", &RadioVal, 1);

			if (ImGui::Button("Connect")) {
				if(ssh.init_session(RadioVal)) LogedIn = true;
			}

			ImGui::Separator();
			ImGui::Text("Config");

			if (ImGui::Button("Reload config.json")) {
				ssh.setCnfList();
			}

			ImGui::SameLine();

			if (ImGui::BeginListBox("ConfigListBox", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {

				cnt = 0;

				std::vector<std::string>::iterator ListIterator;

				for (ListIterator = ssh.cnfList.begin(); ListIterator != ssh.cnfList.end(); ListIterator++) {
					const bool is_selected = (CurrentIdx == cnt);

					if (ImGui::Selectable((*ListIterator).c_str(), is_selected)) {
						ImGui::SetItemDefaultFocus();
						CurrentIdx = cnt;
						ssh.LoadCnf((*ListIterator).c_str());
					}
					cnt++;
				}
				ImGui::EndListBox();
			}
			ImGui::End();
		}

		if(LogedIn) {
			SHELL::parseANSICodes(ssh.Read());
			ImGui::Begin("SSH"); 
			SHELL::Render("SSH", ssh.channel);
			ImGui::End();
		}

		// Rendering
		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}

	libssh2_exit();

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync
	// g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}
	if (g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
