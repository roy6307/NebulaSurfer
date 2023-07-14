/*
Base source from imgui\examples\example_win32_directx9\main.cpp
*/

#ifndef IMGUI_VERSION
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_internal.h"
#endif

#ifndef __NEBULASURFER_NETWORK__
#include "network.h"
#endif

#ifndef __NEBULASURFER_SHELL__
#include "shell.h"
#endif

#ifndef __NEBULASURFER_EXPLORER__
#include "explorer.h"
#endif

#ifndef _CSTDLIB_
#include <cstdlib>
#endif

#ifndef _D3D9_H_
#include <d3d9.h>
#endif

#ifndef _INC_TCHAR
#include <tchar.h>
#endif

#ifndef _WINDOWS_
#include <Windows.h>
#endif

// Data
static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifdef _DEBUG
int main(int, char **)
#else
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
#endif
{
	// Create application window
	// ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"NebularSurfer", NULL};
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
	ImGuiIO &io = ImGui::GetIO();
	(void)io;

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// ImVector<ImWchar> ranges;
	// ImFontGlyphRangesBuilder builder;

	// io.Fonts->AddFontFromFileTTF("fonts/NotoSansKR-Medium.otf", 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());
	//  io.Fonts->AddFontFromFileTTF("fonts/NotoSansJP-Medium.otf", 16.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//  io.Fonts->AddFontFromFileTTF("fonts/NotoSansTC-Medium.otf", 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	//  io.Fonts->Build();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// NebulaSurfer::Explorer::init(g_pd3dDevice);

	// Main loop
	bool done = false;

	static bool LogedIn = false;
	static bool showSSH = false;
	static bool showSFTP = false;

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

		bool open = true;

		ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);

		// based on https://github.com/ocornut/imgui/issues/2109#issuecomment-430096134
		/*
		cases
		1. clicked servers.  showSSH, showSFTP is false. show servers config window. use only one more dock
		2. click server(nickname or host ip etc.) showSSH is true, show SFTP is false. use only one more dock
		3. click SFTP. showSSH is false, show SFTP is true. use only two more dock. List | Local explorer | Remote explorer

		Currently available List, Home, Remote, Local
		*/
		ImGui::Begin("MainDockSpace", &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus);
		if (ImGui::DockBuilderGetNode(ImGui::GetID("MyDockspace")) == NULL)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
			ImGui::DockBuilderAddNode(dockspace_id);	// Add empty node

			ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
			ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);

			ImGui::DockBuilderDockWindow("List", dock_id_left);

			// when click "servers" button.
			// if (showSSH == false && showSFTP == false)
			{
				ImGuiID dock_id_home = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
				ImGui::DockBuilderDockWindow("Home", dock_id_home);
			}

			// when clicked server button.
			// if (showSSH == true && showSFTP == false)
			{
				ImGuiID dock_id_ssh = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
				ImGui::DockBuilderDockWindow("SSH", dock_id_ssh);
			}

			// when clicked sftp button.
			// if (showSSH == false && showSFTP == true)
			{
				ImGuiID dock_id_expRemote = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
				ImGuiID dock_id_expLocal = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
				ImGui::DockBuilderDockWindow("Remote", dock_id_expRemote);
				ImGui::DockBuilderDockWindow("Local", dock_id_expLocal);
			}

			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoDockingInCentralNode);
		ImGui::End();

		if (LogedIn == true)
			NebulaSurfer::SHELL::parseANSICodes(NebulaSurfer::Network::SSH::Read());

		{ // Dock: List
			ImGui::Begin("List", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus);

			if (ImGui::Button("Servers"))
			{
				showSFTP = false;
				showSSH = false;
			}

			if (ImGui::Button("SFTP") && LogedIn)
			{
				showSFTP = true;
				showSSH = false;
			}

			ImGui::Separator();

			// use image button with Text(server nickname or like ubuntu@127.0.0.1:22)
			// need server index. and edit network.cpp.
			if (ImGui::Button("Ubuntu")) // SSH
			{
				showSSH = true;
				showSFTP = false;
			}

			ImGui::End();
		}

		{											 // Dock: SSH
			if (LogedIn == false && showSSH == true) // show config window if not logedin
			{

				static std::string hostaddr, username, password, pathToPem;
				static int port;

				ImGui::Begin("SSH", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus);
				ImGui::Text("SSH config");
				ImGui::InputText("Host address(ipv4)", &hostaddr);
				ImGui::InputInt("Port", &port);
				ImGui::InputText("Username", &username);
				ImGui::InputText("Password", &password);
				ImGui::InputText("Path to *.pem key", &pathToPem);

				if (ImGui::Button("Connect"))
				{
					if (NebulaSurfer::Network::init(hostaddr.c_str(), port, username.c_str(), password.c_str(), pathToPem.c_str()))
						LogedIn = true;
				}

				ImGui::End();
			}
			else if (LogedIn == true && showSSH == true) // show shell if logedin
			{
				ImGui::Begin("SSH", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus);
				NebulaSurfer::SHELL::Render("SSH");
				ImGui::End();
			}
		}

		if (showSSH == false && showSFTP == false)
		{
			// will be placed hosts that not connected.
			ImGui::Begin("Home", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus);
			ImGui::Text("Home");
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
