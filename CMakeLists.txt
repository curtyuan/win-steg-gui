cmake_minimum_required(VERSION 3.14)

project(win-gui-steg
    VERSION 0.0.1
    DESCRIPTION "A GUI picture examine tool"
    LANGUAGES CXX RC
)

# Force set vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "D:/2_Tools-Installed/vcpkg/scripts/buildsystems/vcpkg.cmake" 
    CACHE STRING "Vcpkg toolchain file")
set(VCPKG_ROOT "D:/2_Tools-Installed/vcpkg" CACHE STRING "Vcpkg root directory")

# Set C++ standard to match requirements
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(FLTK CONFIG REQUIRED)
find_package(JPEG REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fms-compatibility)
endif()

# Add main executable and tests
add_subdirectory(src)
add_subdirectory(tests)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
        PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()





