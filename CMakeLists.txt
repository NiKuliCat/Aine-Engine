cmake_minimum_required(VERSION 3.26)

project(
    AineEngine
    VERSION 0.0.1
    DESCRIPTION "A game engine built with SDL3 and Vulkan"
    LANGUAGES CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


option(AINE_BUILD_EDITOR "Build editor target" ON)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")


add_library(aine_warnings INTERFACE)
add_library(Aine::Warnings ALIAS aine_warnings)

target_compile_features(aine_warnings INTERFACE cxx_std_23)

if(MSVC)
    target_compile_options(
        aine_warnings
        INTERFACE
            /W4
            /permissive-
            /utf-8
            /Zc:__cplusplus
            /MP
    )
else()
    target_compile_options(
        aine_warnings
        INTERFACE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()



add_subdirectory(third-party)
add_subdirectory(engine)

if(AINE_BUILD_EDITOR)
    add_subdirectory(editor)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT AineEditor)
endif()