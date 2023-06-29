// A explorer for local machine.

#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include "imgui/imgui.h"

enum ExplorerFileType {

    DIRECTORY,
    UNKNOWN,
    IMAGE,
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
    std::string currentPath = "C:/";
};

namespace Explorer{

    namespace Local{

        void Render(const char*);

    }

}