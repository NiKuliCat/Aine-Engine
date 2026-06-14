set(AINE_ENGINE_SOURCES 
    "src/Aine/Core/Application.cpp"
    "src/Aine/Core/Log.cpp"
    
  "src/Aine/Platform/Window/SDLWindow.cpp" "src/Aine/Core/Time.cpp" "src/Aine/Platform/Vulkan/VulkanContext.cpp" "src/Aine/Render/Renderer.cpp")

set(AINE_ENGINE_HEADERS
    "include/Aine/Core/Application.h"
    "include/Aine/Core/Core.h"
    "include/Aine/Core/Log.h"
    "include/Aine/Core/Window.h" 
    "include/Platform/Window/SDLWindow.h" 
    "include/Aine/Event/Event.h" 
    "include/Aine/Event/ApplicationEvent.h" 
    "include/Aine/Core/Time.h" "include/Aine/Render/Renderer.h" "include/Platform/Render/VulkanContext.h" "include/Aine/Render/RHIContext.h" "include/Platform/Render/VulkanUtils.h")

add_library(AineEngine STATIC)
add_library(Aine::Engine ALIAS AineEngine)


target_sources(
    AineEngine
    PRIVATE
        ${AINE_ENGINE_SOURCES}
        ${AINE_ENGINE_HEADERS}
)

target_include_directories(
    AineEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Aine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Platform
)

target_link_libraries(
    AineEngine
    PUBLIC
        Aine::Warnings   
    PRIVATE
        spdlog::spdlog
        SDL3::SDL3
        Vulkan::Vulkan
)

target_compile_definitions(
    AineEngine

    PUBLIC
        $<$<CONFIG:Debug>:AINE_DEBUG>
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    ${AINE_ENGINE_SOURCES}
    ${AINE_ENGINE_HEADERS}
)

set_target_properties(
    AineEngine
    PROPERTIES
        OUTPUT_NAME "AineEngine"
        FOLDER "engine"
)