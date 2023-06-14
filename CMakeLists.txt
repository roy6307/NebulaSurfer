cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(NebulaSurfer CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################

include(FetchContent)

FetchContent_Declare(
  libssh2
  GIT_REPOSITORY "https://github.com/libssh2/libssh2"
  GIT_TAG "libssh2-1.11.0"
)

message(ASDASD:${libssh2_SOURCE_DIR})

FetchContent_MakeAvailable(libssh2)


set(PROJECT_NAME NebulaSurfer)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "explorer.h"
    "shell.h"
    "ssh.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "explorer.cpp"
    "NebularSurfer.cpp"
    "shell.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(imgui
    "dependencies/imgui/imconfig.h"
    "dependencies/imgui/imgui.cpp"
    "dependencies/imgui/imgui.h"
    "dependencies/imgui/imgui_demo.cpp"
    "dependencies/imgui/imgui_draw.cpp"
    "dependencies/imgui/imgui_impl_dx9.cpp"
    "dependencies/imgui/imgui_impl_dx9.h"
    "dependencies/imgui/imgui_impl_win32.cpp"
    "dependencies/imgui/imgui_impl_win32.h"
    "dependencies/imgui/imgui_internal.h"
    "dependencies/imgui/imgui_stdlib.cpp"
    "dependencies/imgui/imgui_stdlib.h"
    "dependencies/imgui/imgui_tables.cpp"
    "dependencies/imgui/imgui_widgets.cpp"
    "dependencies/imgui/imstb_rectpack.h"
    "dependencies/imgui/imstb_textedit.h"
    "dependencies/imgui/imstb_truetype.h"
    "dependencies/imgui/LICENSE.txt"
)
source_group("imgui" FILES ${imgui})

set(libssh2
    "dependencies/libssh2/include/libssh2.h"
    "dependencies/libssh2/include/libssh2_publickey.h"
    "dependencies/libssh2/include/libssh2_sftp.h"
)
source_group("libssh2" FILES ${libssh2})

set(rapidjson
    "dependencies/rapidjson/allocators.h"
    "dependencies/rapidjson/cursorstreamwrapper.h"
    "dependencies/rapidjson/document.h"
    "dependencies/rapidjson/encodedstream.h"
    "dependencies/rapidjson/encodings.h"
    "dependencies/rapidjson/filereadstream.h"
    "dependencies/rapidjson/filewritestream.h"
    "dependencies/rapidjson/fwd.h"
    "dependencies/rapidjson/istreamwrapper.h"
    "dependencies/rapidjson/memorybuffer.h"
    "dependencies/rapidjson/memorystream.h"
    "dependencies/rapidjson/ostreamwrapper.h"
    "dependencies/rapidjson/pointer.h"
    "dependencies/rapidjson/prettywriter.h"
    "dependencies/rapidjson/rapidjson.h"
    "dependencies/rapidjson/reader.h"
    "dependencies/rapidjson/schema.h"
    "dependencies/rapidjson/stream.h"
    "dependencies/rapidjson/stringbuffer.h"
    "dependencies/rapidjson/uri.h"
    "dependencies/rapidjson/writer.h"
)
source_group("rapidjson" FILES ${rapidjson})

set(rapidjson__error
    "dependencies/rapidjson/error/en.h"
    "dependencies/rapidjson/error/error.h"
)
source_group("rapidjson\\error" FILES ${rapidjson__error})

set(rapidjson__internal
    "dependencies/rapidjson/internal/biginteger.h"
    "dependencies/rapidjson/internal/clzll.h"
    "dependencies/rapidjson/internal/diyfp.h"
    "dependencies/rapidjson/internal/dtoa.h"
    "dependencies/rapidjson/internal/ieee754.h"
    "dependencies/rapidjson/internal/itoa.h"
    "dependencies/rapidjson/internal/meta.h"
    "dependencies/rapidjson/internal/pow10.h"
    "dependencies/rapidjson/internal/regex.h"
    "dependencies/rapidjson/internal/stack.h"
    "dependencies/rapidjson/internal/strfunc.h"
    "dependencies/rapidjson/internal/strtod.h"
    "dependencies/rapidjson/internal/swap.h"
)
source_group("rapidjson\\internal" FILES ${rapidjson__internal})

set(rapidjson__msinttypes
    "dependencies/rapidjson/msinttypes/inttypes.h"
    "dependencies/rapidjson/msinttypes/stdint.h"
)
source_group("rapidjson\\msinttypes" FILES ${rapidjson__msinttypes})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    ${imgui}
    ${libssh2}
    ${rapidjson}
    ${rapidjson__error}
    ${rapidjson__internal}
    ${rapidjson__msinttypes}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE NebulaSurfer)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.;"
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_WINSOCK_DEPRECATED_NO_WARNINGS;"
        "WIN32_LEAN_AND_MEAN;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Ob2;
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++17;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /SUBSYSTEM:CONSOLE
            >
            $<$<CONFIG:Release>:
                /SUBSYSTEM:WINDOWS;
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Release>:"
            "$(CoreLibraryDependencies)"
        ">"
        "ws2_32;"
        "d3d9;"
        "${libssh2_BINARY_DIR}/src/Debug/libssh2.lib"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

