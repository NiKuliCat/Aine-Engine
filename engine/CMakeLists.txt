set(AINE_ENGINE_SOURCES 
    "src/Aine/Core/Application.cpp"
    "src/Aine/Core/Log.cpp"
    
)

set(AINE_ENGINE_HEADERS
    "include/Aine/Core/Application.h"
    "include/Aine/Core/Core.h"
    "include/Aine/Core/Log.h"
)

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
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    AineEngine
    PUBLIC
        Aine::Warnings   
    PRIVATE
        spdlog::spdlog
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