// A explorer for local machine.

#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <d3d9.h>
#include <DirectXTex.h>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"


enum ExplorerFileType {

    FOLDER,
    DEFAULT,
    PICTURE,
    TXT,
    EXE

};

struct ExplorerFileInfo {

    unsigned long long size = 0;
    std::filesystem::path path = ""; // use absolute path
    ExplorerFileType ftype;

};

struct ExplorerMain {
    std::vector<ExplorerFileInfo> flist;
    std::string currentPath = "C:\\";
};

namespace Explorer{

    void init(LPDIRECT3DDEVICE9);

    namespace Local{

        void Render(const char*);

    }

}