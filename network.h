#pragma once

#ifndef NEBULASURFER_NETWORK_H
#define NEBULASURFER_NETWORK_H
#endif

#ifndef LIBSSH2_H
#include "libssh2/include/libssh2.h"
#endif

#ifndef LIBSSH2_SFTP_H
#include "libssh2/include/libssh2_sftp.h"
#endif

#ifndef _CSTDIO_
#include <cstdio>
#endif

#ifndef _VECTOR_
#include <vector>
#endif

#ifndef _STRING_
#include <string>
#endif

#ifndef _FSTREAM_
#include <fstream>
#endif

#ifndef _IOSTREAM_
#include <iostream>
#endif

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

            bool Download(std::string src, std::string dst);
            bool Upload(std::string src, std::string dst);
            void Read();
            void Mkdir(const char* path);
            bool List(const char *path, std::vector<std::string>* fnames, std::vector<std::string>* fpaths, std::vector<unsigned long long>* fsizes, std::vector<bool>* isDir);

        }

    }
}