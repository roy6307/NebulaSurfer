#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include "libssh2/include/libssh2.h"
#include "libssh2/include/libssh2_sftp.h"

namespace NebulaSurfer
{

    namespace Network
    {
        
        // init ssh, sftp
        bool init(const char* hostaddr, int port, const char* username, const char* password, const char* pathToPem);

        namespace SSH
        {

            std::string Read();
            void Exec(const char* buf);

        }

        namespace SFTP
        {

            void Write();
            void Read();
            void Mkdir(const char* path);
            bool List(const char *path, std::vector<std::string>* fnames, std::vector<std::string>* fpaths, std::vector<unsigned long long>* fsizes, std::vector<bool>* isDir);

        }

    }
}