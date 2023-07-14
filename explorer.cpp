#include "explorer.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "dependencies/stb/stb_image.h"
#endif

static ExplorerMain em;
static std::vector<LPDIRECT3DTEXTURE9> textures;
static bool initialized = false;
static bool needLocalFlistUpdate = true;
static bool needRemoteFlistUpdate = true;

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
void NebulaSurfer::Explorer::init(LPDIRECT3DDEVICE9 device)
{

    if (initialized == false)
    {
        // use va_args instead
        const char *image_folder = "./images/folder.png";
        const char *image_default = "./images/default.png";
        const char *image_picture = "./images/picture-1.png";
        const char *image_txt = "./images/txt.png";
        const char *image_exe = "./images/exe.png";

        LoadAndCreateTexture(device, image_folder);
        LoadAndCreateTexture(device, image_default);
        LoadAndCreateTexture(device, image_picture);
        LoadAndCreateTexture(device, image_txt);
        LoadAndCreateTexture(device, image_exe);

        initialized = true;
    }
}

// LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
//**********************   LOCAL PART   ****************************
// LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL

void localFlistUpdate()
{
    em.lflist.clear();

    for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(em.currentLocalPath))
    {
        ExplorerFileInfo tefi;
        tefi.path = entry.path().generic_u8string();
        tefi.size = entry.file_size();
        tefi.filename = entry.path().filename().generic_u8string();

        if (false == entry.is_directory())
        {
            std::string comp = entry.path().extension().generic_u8string();

            if (comp == ".png" || comp == ".jpg" || comp == ".jpeg" || comp == ".gif" || comp == ".webp")
                tefi.ftype = ExplorerFileType::PICTURE;
            else if (comp == ".txt")
                tefi.ftype = ExplorerFileType::TXT;
            else if (comp == ".exe")
                tefi.ftype = ExplorerFileType::EXE;
            else
                tefi.ftype = ExplorerFileType::DEFAULT;
        }
        else
        {
            tefi.ftype = ExplorerFileType::FOLDER;
        }

        em.lflist.push_back(tefi);
    }
}

void NebulaSurfer::Explorer::Local::Render(const char *title)
{
    std::error_code ec;
    const std::filesystem::space_info si = std::filesystem::space("C:/", ec);

    if (needLocalFlistUpdate)
    {
        localFlistUpdate();
        needLocalFlistUpdate = false;
    }

    ImGui::SetNextWindowSize(ImVec2(825, 490));
    ImGui::Begin(title, 0, ImGuiWindowFlags_NoResize);
    {

        ImGui::SetCursorPos(ImVec2(15, 35));
        ImGui::BeginChild("TimeDisplay", ImVec2(190, 84), true);
        {

            ImGui::SetCursorPos(ImVec2(60.5, 36.5));
            ImGui::Text("Time");
        }

        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(15, 135));
        ImGui::BeginChild("RecentlyAccessed", ImVec2(220, 154), true);
        {

            ImGui::SetCursorPos(ImVec2(35, 10));
            ImGui::Text("Recent");

            ImGui::SetCursorPos(ImVec2(50, 40));
            ImGui::PushItemWidth(160);
            static int item_current4 = 0;
            const char *items4[] = {"AAAA", "BBBB", "CCCC", "DDDD"};
            ImGui::ListBox("##RecentDir", &item_current4, items4, IM_ARRAYSIZE(items4));
            ImGui::PopItemWidth();
        }

        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(15, 305));
        ImGui::BeginChild("Drives", ImVec2(220, 169), true);
        {

            ImGui::SetCursorPos(ImVec2(35, 15));
            ImGui::Text("Drive");

            ImGui::SetCursorPos(ImVec2(50, 45));
            if (ImGui::Button("C:/", {0, 0}))
            {
                em.currentLocalPath = "/";
                needLocalFlistUpdate = true;
            }
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(220, 35));
        ImGui::BeginChild("TopBar", ImVec2(590, 84), true);
        {
            // ImGui::SetCursorPos(ImVec2(15, 10));
            std::filesystem::path pbase;

            for (auto &splited : std::filesystem::path(em.currentLocalPath))
            {
                pbase /= splited;
                // pbase += "/";
                if (splited.generic_u8string() != "C:")
                {
                    if (ImGui::Button((splited.generic_u8string() + "/").c_str(), {0, 0}))
                    {
                        // std::cout << "Hit" <<std::endl;
                        std::cout << pbase << std::endl;
                        em.currentLocalPath = std::filesystem::absolute(pbase).generic_u8string()+"/"; // << std::endl;
                        needLocalFlistUpdate = true;
                        // std::cout << std::filesystem::canonical(splited).generic_u8string() << std::endl;
                    }
                }

                ImGui::SameLine();
            }

            ImGui::SetCursorPos(ImVec2(15, 45.5));
            ImGui::ArrowButton("##backward", ImGuiDir_Left);
            ImGui::SetCursorPos(ImVec2(45, 45.5));
            ImGui::ArrowButton("##foreward", ImGuiDir_Right);

            ImGui::SetCursorPos(ImVec2(170, 45));
            ImGui::ArrowButton("##ascending", ImGuiDir_Up);
            ImGui::SameLine();
            ImGui::ArrowButton("##descending", ImGuiDir_Down);

            ImGui::SetCursorPos(ImVec2(225.5, 47));
            ImGui::Text("A-Z");

            ImGui::SetCursorPos(ImVec2(265, 45));
            ImGui::Button("  ", ImVec2(22, 19)); // add folder

            ImGui::SetCursorPos(ImVec2(295, 45));
            ImGui::Button("  ", ImVec2(22, 19)); // add file

            ImGui::SetCursorPos(ImVec2(325, 45));
            ImGui::Button("  ", ImVec2(22, 19)); // search

            ImGui::SetCursorPos(ImVec2(415.5, 17.5));
            ImGui::Text("The drive used:");

            ImGui::SetCursorPos(ImVec2(370, 45));
            ImGui::PushItemWidth(200);

            uintmax_t availSpa = si.available / (1 << 20); // MB
            uintmax_t capaSpa = si.capacity / (1 << 20);   // MB
            uintmax_t subSpa = capaSpa - availSpa;
            double calcedSpa = (double)subSpa / (double)capaSpa;
            // std::cout << calcedSpa <<std::endl;
            ImGui::ProgressBar(calcedSpa, ImVec2(0.0f, 0.0f)); // drive usage
            ImGui::PopItemWidth();
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(250, 135));
        ImGui::BeginChild("##filelist", ImVec2(560, 339), true);
        {

            for (int n = 0; n < em.lflist.size(); n++)
            {
                ImGui::PushID(n);
                if ((n % 5) != 0)
                    ImGui::SameLine();

                int tttttt = em.lflist.at(n).ftype;
                LPDIRECT3DTEXTURE9 ttex = textures.at(tttttt);

                ImDrawList *draw_list = ImGui::GetWindowDrawList();
                ImGuiWindow *window = ImGui::GetCurrentWindow();
                ImVec2 bef = window->DC.CursorPos;
                std::string tteexxtt = em.lflist.at(n).filename;
                float wid = ImGui::CalcTextSize(tteexxtt.c_str()).x;

                if (ImGui::ImageButton((void *)ttex, ImVec2(84, 84)))
                {
                    if (em.lflist.at(n).ftype == ExplorerFileType::FOLDER)
                        em.currentLocalPath = std::filesystem::canonical(em.lflist.at(n).path).generic_u8string()+"/";
                    needLocalFlistUpdate = true;
                }

                draw_list->AddText(ImVec2((2 * bef.x + 84.0f - wid) * 0.5f, bef.y + 73.5f), ImColor(1.0f, 1.0f, 1.0f, 1.0f), tteexxtt.c_str());
                // Our buttons are both drag sources and drag targets here!
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    // Set payload to carry the index of our item (could be anything)
                    ImGui::SetDragDropPayload("flis", &n, sizeof(int));

                    // Display preview (could be anything, e.g. when dragging an image we could decide to display
                    // the filename and a small preview of the image, etc.)
                    {
                        ImGui::Text("Moving %s", em.lflist.at(n).filename);
                    }
                    ImGui::EndDragDropSource();
                }
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("flis"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int *)payload->Data;
                        {
                            em.lflist.at(n) = em.lflist.at(payload_n);
                            em.lflist.erase(em.lflist.begin() + payload_n);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

// RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//**********************   REMOTE PART   ***************************
// RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR

bool remoteFlistUpdate()
{

    std::vector<std::string> fns, fpths;
    std::vector<unsigned long long> fszs;
    std::vector<bool> idr;

    em.rflist.clear();

    bool res = NebulaSurfer::Network::SFTP::List(em.currentRemotePath.c_str(), &fns, &fpths, &fszs, &idr);

    if (res == true)
    {
        size_t i = fns.size();

        for (size_t j = 0; j < i; j++)
        {
            ExplorerFileInfo tefi;
            std::string fextension = std::filesystem::path(fns.at(j)).extension().generic_u8string();

            if (idr.at(j) == false)
            {
                if (fextension == ".png" || fextension == ".jpg" || fextension == ".jpeg" || fextension == ".gif" || fextension == ".webp")
                    tefi.ftype = ExplorerFileType::PICTURE;
                else if (fextension == ".txt")
                    tefi.ftype = ExplorerFileType::TXT;
                else if (fextension == ".exe")
                    tefi.ftype = ExplorerFileType::EXE;
                else
                    tefi.ftype = ExplorerFileType::DEFAULT;
            }
            else
                tefi.ftype = ExplorerFileType::FOLDER;

            tefi.filename = fns.at(j);
            tefi.size = fszs.at(j);
            tefi.path = em.currentRemotePath + fns.at(j);

            em.rflist.push_back(tefi);
        }

        return true;
    }
    else
        return false;
}

void NebulaSurfer::Explorer::Remote::Render(const char *title)
{

    if (needRemoteFlistUpdate)
    {
        if (remoteFlistUpdate())
            needRemoteFlistUpdate = false;
    }

    ImGui::Begin(title);
    {

        static uint64_t ddd = 0;

        for (int n = 0; n < em.rflist.size(); n++)
        {
            ImGui::PushID(n);

            if ((n % 5) != 0)
                ImGui::SameLine();

            LPDIRECT3DTEXTURE9 ttex = textures.at(em.rflist.at(n).ftype);

            ImDrawList *draw_list = ImGui::GetWindowDrawList();
            ImGuiWindow *window = ImGui::GetCurrentWindow();
            ImVec2 bef = window->DC.CursorPos;
            const char *ftext = em.rflist.at(n).filename.c_str();
            float wid = ImGui::CalcTextSize(ftext).x;

            if (ImGui::ImageButton((void *)ttex, ImVec2(84, 84)))
            {
                // std::cout << "Hit\n";
                if (em.rflist.at(n).ftype == ExplorerFileType::FOLDER && em.rflist.at(n).filename != ".")
                {

                    if (em.rflist.at(n).filename == "..")
                    {
                        em.currentRemotePath = "/";
                        needRemoteFlistUpdate = true;
                    }
                    else
                    {
                        em.previousRemotePath = em.currentRemotePath;
                        em.currentRemotePath += em.rflist.at(n).filename + "/";
                        // std::cout << em.currentRemotePath << std::endl;
                        needRemoteFlistUpdate = true;
                    }
                }
                else if (em.rflist.at(n).ftype != ExplorerFileType::FOLDER)
                {
                    std::future<bool> a = std::async(std::launch::async, NebulaSurfer::Network::SFTP::Download, em.rflist.at(n).path, em.currentLocalPath+em.rflist.at(n).filename);
                }
            }

            draw_list->AddText(ImVec2((2 * bef.x + 84.0f - wid) * 0.5f, bef.y + 73.5f), ImColor(1.0f, 1.0f, 1.0f, 1.0f), ftext);

            ImGui::PopID();
        }
    }
    ImGui::End();
}