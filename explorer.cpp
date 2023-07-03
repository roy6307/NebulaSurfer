#include "explorer.h"
#include <iostream>
#ifndef STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#include "dependencies/stb/stb_image.h"

#endif

static ExplorerMain em;
static std::vector<LPDIRECT3DTEXTURE9> textures;
static bool initialized = false;

void LoadAndCreateTexture(LPDIRECT3DDEVICE9 device, const char *fname)
{

    LPDIRECT3DTEXTURE9 texture = nullptr;
    int width, height, channels;
    unsigned char *image = stbi_load(fname, &width, &height, &channels, STBI_rgb_alpha);
    if (image)
    {
        HRESULT result = device->CreateTexture(
            width,
            height,
            1,
            D3DUSAGE_DYNAMIC,
            D3DFMT_A8R8G8B8,
            D3DPOOL_DEFAULT,
            &texture,
            nullptr);
        if (SUCCEEDED(result))
        {
            D3DLOCKED_RECT lockedRect;
            texture->LockRect(0, &lockedRect, nullptr, D3DLOCK_DISCARD);

            unsigned char *dest = static_cast<unsigned char *>(lockedRect.pBits);

            for (int y = 0; y < height; ++y)
            {
                memcpy(dest, image + (y * width * 4), width * 4);
                dest += lockedRect.Pitch;
            }

            texture->UnlockRect(0);

            textures.push_back(texture);
        }

        stbi_image_free(image);
    }
}

// Load images and create textures.
void Explorer::init(LPDIRECT3DDEVICE9 device)
{

    if (initialized == false)
    {

        const char* image_folder = "./images/folder.png";
        const char* image_default = "./images/default.png";
        const char* image_picture = "./images/picture-1.png";
        const char* image_txt = "./images/txt.png";
        const char* image_exe = "./images/exe.png";

        LoadAndCreateTexture(device, image_folder);
        LoadAndCreateTexture(device, image_default);
        LoadAndCreateTexture(device, image_picture);
        LoadAndCreateTexture(device, image_txt);
        LoadAndCreateTexture(device, image_exe);
        
        initialized = true;

    }
}

void flistUpdate()
{
    em.flist.clear();

    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(em.currentPath)) {
    ExplorerFileInfo tefi;
    tefi.path = entry.path();
    tefi.size = entry.file_size();

    if(false == entry.is_directory()){
        std::string comp = entry.path().extension().generic_u8string();

        if(comp == ".png" || comp == ".jpg" || comp == ".jpeg") tefi.ftype = ExplorerFileType::PICTURE;
        else if(comp == ".txt") tefi.ftype = ExplorerFileType::TXT;
        else if(comp == ".exe") tefi.ftype = ExplorerFileType::EXE;
        else tefi.ftype = ExplorerFileType::DEFAULT;

    }else{tefi.ftype= ExplorerFileType::FOLDER;}

    em.flist.push_back(tefi);
  }
}

void Explorer::Local::Render(const char *title)
{
    flistUpdate();
    ImGui::Begin(title);
    {
        ImGui::BeginChild("arrows", ImVec2(90, 40), false);
        {
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::ArrowButton("arLeft", ImGuiDir_Left);
            

            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::ArrowButton("arRight", ImGuiDir_Right);


            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::ArrowButton("arUp", ImGuiDir_Up);

            ImGui::EndChild();
        }

        ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
        ImGui::BeginChild("splitedPaths", {-1, 40}, false);
        {

            ImGui::Spacing();
            ImGui::Spacing();

            for (auto &splited : std::filesystem::path(em.currentPath))
            {
                if (ImGui::Button(splited.generic_u8string().c_str(), {0, 0}))
                {
                    std::cout << "Hit" <<std::endl;
                    //em.currentPath = std::filesystem::canonical(splited).generic_u8string();
                }

                ImGui::SameLine();
            }

            ImGui::EndChild();
        }

        // split(em.currentPath, '/')
        ImGui::BeginChild("drives", {70, -1}, false);
        {
            if(ImGui::Button("C:/", {0, 0})){
                em.currentPath = "C:/";
            }

            ImGui::EndChild();
        }

        ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
        ImGui::BeginChild("filelist", {-1, -1}, false);
        {

            for (int n = 0; n < em.flist.size(); n++)
            {
                ImGui::PushID(n);
                if ((n % 7) != 0)
                    ImGui::SameLine();

                int tttttt = em.flist.at(n).ftype;
                LPDIRECT3DTEXTURE9 ttex = textures.at(tttttt);
                
                ImDrawList *draw_list = ImGui::GetWindowDrawList();
			    ImGuiWindow* window = ImGui::GetCurrentWindow();
                ImVec2 bef = window->DC.CursorPos;
                std::string tteexxtt = em.flist.at(n).path.filename().generic_u8string();
                float wid = ImGui::CalcTextSize(tteexxtt.c_str()).x;

                if(ImGui::ImageButton((void*)ttex, ImVec2(84,84))){
                    if(em.flist.at(n).ftype == ExplorerFileType::FOLDER) em.currentPath = std::filesystem::canonical(em.flist.at(n).path).generic_u8string();
                }

                draw_list->AddText(ImVec2((2*bef.x+84.0f-wid)*0.5f, bef.y+73.5f), ImColor(1.0f,1.0f,1.0f,1.0f), tteexxtt.c_str());
                // Our buttons are both drag sources and drag targets here!
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    // Set payload to carry the index of our item (could be anything)
                    ImGui::SetDragDropPayload("flis", &n, sizeof(int));

                    // Display preview (could be anything, e.g. when dragging an image we could decide to display
                    // the filename and a small preview of the image, etc.)
                    { ImGui::Text("Moving %s", em.flist.at(n).path.filename().generic_u8string().c_str()); }
                    ImGui::EndDragDropSource();
                }
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("flis"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int*)payload->Data;
                        {
                            em.flist.at(n) = em.flist.at(payload_n);
                            em.flist.erase(em.flist.begin()+payload_n);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
            }

            ImGui::EndChild();
        }
        ImGui::End();
    }
}