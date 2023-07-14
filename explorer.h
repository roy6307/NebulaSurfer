// A explorer for local machine.

#pragma once

#ifndef NEBULASURFER_EXPLORER_H
#define NEBULASURFER_EXPLORER_H
#endif

#ifndef IMGUI_VERSION
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#endif

#ifndef NEBULASURFER_NETWORK_H
#include "network.h"
#endif

#ifndef _VECTOR_
#include <vector>
#endif

#ifndef _STRING_
#include <string>
#endif

#ifndef _FILESYSTEM_
#include <filesystem>
#endif

#ifndef _D3D9_H_
#include <d3d9.h>
#endif

#ifndef _IOSTREAM_
#include <iostream>
#endif

#ifndef _FUTURE_
#include <future>
#endif

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
    std::string path = ""; // use absolute path
    std::string filename = "";
    ExplorerFileType ftype;
};

struct ExplorerMain
{
    std::vector<ExplorerFileInfo> lflist; // for local
    std::vector<ExplorerFileInfo> rflist; // for remote
    std::string currentLocalPath = "/";
    std::string currentRemotePath = "/";
    std::string previousRemotePath = "/";
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