set(CMAKE_CXX_COMPILER /usr/bin/g++-11 CACHE STRING "GCC 11 override")

if(NOT _VCPKG_LINUX_TOOLCHAIN)
    set(_VCPKG_LINUX_TOOLCHAIN 1)

    if(POLICY CMP0056)
        cmake_policy(SET CMP0056 NEW)
    endif()
    if(POLICY CMP0066)
        cmake_policy(SET CMP0066 NEW)
    endif()
    if(POLICY CMP0067)
        cmake_policy(SET CMP0067 NEW)
    endif()
    if(POLICY CMP0137)
        cmake_policy(SET CMP0137 NEW)
    endif()
    list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
        VCPKG_CRT_LINKAGE VCPKG_TARGET_ARCHITECTURE
        VCPKG_C_FLAGS VCPKG_CXX_FLAGS
        VCPKG_C_FLAGS_DEBUG VCPKG_CXX_FLAGS_DEBUG
        VCPKG_C_FLAGS_RELEASE VCPKG_CXX_FLAGS_RELEASE
        VCPKG_LINKER_FLAGS VCPKG_LINKER_FLAGS_RELEASE VCPKG_LINKER_FLAGS_DEBUG
    )

    set(CMAKE_SYSTEM_NAME Linux CACHE STRING "")
    if(VCPKG_TARGET_ARCHITECTURE STREQUAL "x64")
       set(CMAKE_SYSTEM_PROCESSOR x86_64 CACHE STRING "")
    elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL "x86")
       set(CMAKE_SYSTEM_PROCESSOR i686 CACHE STRING "")
       string(APPEND VCPKG_C_FLAGS " -m32")
       string(APPEND VCPKG_CXX_FLAGS " -m32")
       string(APPEND VCPKG_LINKER_FLAGS " -m32")
    elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL "arm")
        set(CMAKE_SYSTEM_PROCESSOR armv7l CACHE STRING "")
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND NOT CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "armv7l")

            if(NOT DEFINED CMAKE_CXX_COMPILER)
                set(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
            endif()
            if(NOT DEFINED CMAKE_C_COMPILER)
                set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
            endif()
            if(NOT DEFINED CMAKE_ASM_COMPILER)
                set(CMAKE_ASM_COMPILER "arm-linux-gnueabihf-gcc")
            endif()
            if(NOT DEFINED CMAKE_ASM-ATT_COMPILER)
                set(CMAKE_ASM-ATT_COMPILER "arm-linux-gnueabihf-as")
            endif()
            message(STATUS "Cross compiling arm on host ${CMAKE_HOST_SYSTEM_PROCESSOR}, use cross compiler: ${CMAKE_CXX_COMPILER}/${CMAKE_C_COMPILER}")
        endif()
    elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL "arm64")
        set(CMAKE_SYSTEM_PROCESSOR aarch64 CACHE STRING "")
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND NOT CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "aarch64")

            if(NOT DEFINED CMAKE_CXX_COMPILER)
                set(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++")
            endif()
            if(NOT DEFINED CMAKE_C_COMPILER)
                set(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc")
            endif()
            if(NOT DEFINED CMAKE_ASM_COMPILER)
                set(CMAKE_ASM_COMPILER "aarch64-linux-gnu-gcc")
            endif()
            if(NOT DEFINED CMAKE_ASM-ATT_COMPILER)
                set(CMAKE_ASM-ATT_COMPILER "aarch64-linux-gnu-as")
            endif()
            message(STATUS "Cross compiling arm64 on host ${CMAKE_HOST_SYSTEM_PROCESSOR}, use cross compiler: ${CMAKE_CXX_COMPILER}/${CMAKE_C_COMPILER}")
        endif()
    endif()

    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR STREQUAL CMAKE_HOST_SYSTEM_PROCESSOR)
        set(CMAKE_CROSSCOMPILING OFF CACHE BOOL "")
    endif()

    string(APPEND CMAKE_C_FLAGS_INIT " -fPIC ${VCPKG_C_FLAGS} ")
    string(APPEND CMAKE_CXX_FLAGS_INIT " -fPIC ${VCPKG_CXX_FLAGS} ")
    string(APPEND CMAKE_C_FLAGS_DEBUG_INIT " ${VCPKG_C_FLAGS_DEBUG} ")
    string(APPEND CMAKE_CXX_FLAGS_DEBUG_INIT " ${VCPKG_CXX_FLAGS_DEBUG} ")
    string(APPEND CMAKE_C_FLAGS_RELEASE_INIT " ${VCPKG_C_FLAGS_RELEASE} ")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE_INIT " ${VCPKG_CXX_FLAGS_RELEASE} ")

    string(APPEND CMAKE_MODULE_LINKER_FLAGS_INIT " ${VCPKG_LINKER_FLAGS} ")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " ${VCPKG_LINKER_FLAGS} ")
    string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " ${VCPKG_LINKER_FLAGS} ")
    if(VCPKG_CRT_LINKAGE STREQUAL "static")
        string(APPEND CMAKE_MODULE_LINKER_FLAGS_INIT "-static ")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT "-static ")
        string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT "-static ")
    endif()
    string(APPEND CMAKE_MODULE_LINKER_FLAGS_DEBUG_INIT " ${VCPKG_LINKER_FLAGS_DEBUG} ")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS_DEBUG_INIT " ${VCPKG_LINKER_FLAGS_DEBUG} ")
    string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG_INIT " ${VCPKG_LINKER_FLAGS_DEBUG} ")
    string(APPEND CMAKE_MODULE_LINKER_FLAGS_RELEASE_INIT " ${VCPKG_LINKER_FLAGS_RELEASE} ")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS_RELEASE_INIT " ${VCPKG_LINKER_FLAGS_RELEASE} ")
    string(APPEND CMAKE_EXE_LINKER_FLAGS_RELEASE_INIT " ${VCPKG_LINKER_FLAGS_RELEASE} ")
    string(APPEND CMAKE_ASM_FLAGS_INIT " ${VCPKG_C_FLAGS} ")
endif()