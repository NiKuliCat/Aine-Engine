set(AINE_EDITOR_SOURCES
    src/EditorApplication.cpp
    src/main.cpp
)


set(AINE_EDITOR_HEADERS
    include/EditorApplication.h
)

add_executable(AineEditor)

target_sources(
    AineEditor
    PRIVATE
        ${AINE_EDITOR_SOURCES}
        ${AINE_EDITOR_HEADERS}
)

target_include_directories(
    AineEditor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    AineEditor
    PRIVATE
        Aine::Engine
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    ${AINE_EDITOR_SOURCES}
    ${AINE_EDITOR_HEADERS}
)

set_target_properties(
    AineEditor
    PROPERTIES
        OUTPUT_NAME "AineEditor"
        FOLDER "editor"
)
