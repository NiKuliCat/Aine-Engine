set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)


add_subdirectory(spdlog)


set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(SDL EXCLUDE_FROM_ALL)


find_package(Vulkan REQUIRED)

add_library(imgui STATIC
    ImGui/imgui.cpp
    ImGui/imgui_demo.cpp
    ImGui/imgui_draw.cpp
    ImGui/imgui_tables.cpp
    ImGui/imgui_widgets.cpp
    ImGui/backends/imgui_impl_sdl3.cpp
    ImGui/backends/imgui_impl_vulkan.cpp
)
add_library(ImGui::ImGui ALIAS imgui)

target_include_directories(
    imgui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/ImGui
        ${CMAKE_CURRENT_SOURCE_DIR}/ImGui/backends
)

target_link_libraries(
    imgui
    PUBLIC
        SDL3::SDL3
        Vulkan::Vulkan
)

set_target_properties(
    imgui
    PROPERTIES
        FOLDER "third-party"
)
