![alt text](https://github.com/roy6307/NebulaSurfer/blob/master/images/main.png?raw=true)

# NebulaSurfer
 ##### NebulaSurfer, a free open-source ssh and sftp gui client for windows.
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](LICENSE)
[![Language](https://img.shields.io/badge/Language-C++-red?style=flat-square)](Language)
 
  I'm still working on this project currently.
  The basic ones will be completed as soon as possible. :D
  And I'm looking for someone familiar with CMakeLists.txt.
  If you are the one, please open a issue. (I'm not familiar with it... I need help....)
  
  ### Used libraries
  [![libssh2](https://img.shields.io/badge/%20-libssh2-green?style=for-the-badge&logo=github)](https://github.com/libssh2/libssh2)
  [![imgui](https://img.shields.io/badge/%20-ImGui-yellow?style=for-the-badge&logo=github)](https://github.com/ocornut/imgui)
  [![rapidjson](https://img.shields.io/badge/%20-rapidjson-blue?style=for-the-badge&logo=github)](https://github.com/Tencent/rapidjson)
  [![rapidjson](https://img.shields.io/badge/%20-stb-purple?style=for-the-badge&logo=github)](https://github.com/nothings/stb)
 
 ## Build
   Before use, I am using x64 Windows Kits 10.0.19041.0 for Windows 10 with MSVC compiler.
   OpenSSL is required. If you don't have, please install it.

   If you have all the requirements, please type commands below:
     
     git clone https://github.com/roy6307/NebulaSurfer.git
     cd NebulaSurfer
     mkdir Build
     cd Build
     cmake ..
     cmake --build . --config Debug --target libssh2_shared
     cmake --build . --config Debug --target NebulaSurfer

   If you done, you can find the output at NebulaSurfer/x64/Debug.

 ## Troubleshooting
    1. Problem with libssh2.dll
    Place NebulaSurfer/build/_deps/libssh2-build/src/Debug/libssh2.dll 
    in the same place as to NebulaSurfer.exe

    2. Problem with libcrypto-3-x64.dll
    Copy (Where you installed OpenSSL)/bin/libcrypto-3-x64.dll
    to NebulaSurfer.exe

    3. Some characters look like □
    The language is not currently supported. If you could,
    please open an issue and let me know the language that you use. I'll update ASAP.