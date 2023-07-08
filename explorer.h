// A explorer for local machine.

#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <d3d9.h>
#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "network.h"

enum ExplorerFileType
{
    FOLDER,
    DEFAULT,
    PICTURE,
    TXT,
    EXE

};

struct ExplorerFileInfo
{
    unsigned long long size = 0;
    std::filesystem::path path = ""; // use absolute path
    ExplorerFileType ftype;
};

struct ExplorerMain
{
    std::vector<ExplorerFileInfo> lflist; // for local
    std::vector<ExplorerFileInfo> rflist; // for remote
    std::string currentPath = "/";
};

namespace NebulaSurfer
{

    namespace Explorer
    {

        void init(LPDIRECT3DDEVICE9);

        namespace Local
        {

            void Render(const char *);

        }

        namespace Remote{
            
            void Render(const char* title);

        }
        
    }
}