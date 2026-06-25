cmake_minimum_required(VERSION 3.13)

project(tree-sitter-typescript
        VERSION "0.23.2"
        DESCRIPTION "TypeScript grammar for tree-sitter"
        HOMEPAGE_URL "https://github.com/tree-sitter/tree-sitter-typescript"
        LANGUAGES C)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(TREE_SITTER_REUSE_ALLOCATOR "Reuse the library allocator" OFF)

set(TREE_SITTER_ABI_VERSION 14 CACHE STRING "Tree-sitter ABI version")
if(NOT ${TREE_SITTER_ABI_VERSION} MATCHES "^[0-9]+$")
    unset(TREE_SITTER_ABI_VERSION CACHE)
    message(FATAL_ERROR "TREE_SITTER_ABI_VERSION must be an integer")
endif()

include(GNUInstallDirs)

set(TREE_SITTER_TYPESCRIPT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

macro(add_parser name)
    add_library(tree-sitter-${name}
                "${CMAKE_CURRENT_SOURCE_DIR}/src/parser.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/scanner.c")
    target_include_directories(tree-sitter-${name}
                               PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
                               INTERFACE $<BUILD_INTERFACE:${TREE_SITTER_TYPESCRIPT_ROOT}/bindings/c>
                                         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    target_compile_definitions(tree-sitter-${name} PRIVATE
                               $<$<BOOL:${TREE_SITTER_REUSE_ALLOCATOR}>:TREE_SITTER_REUSE_ALLOCATOR>
                               $<$<CONFIG:Debug>:TREE_SITTER_DEBUG>)

    set_target_properties(tree-sitter-${name}
                          PROPERTIES
                          C_STANDARD 11
                          POSITION_INDEPENDENT_CODE ON
                          SOVERSION "${TREE_SITTER_ABI_VERSION}.${PROJECT_VERSION_MAJOR}"
                          DEFINE_SYMBOL "")

    configure_file("${TREE_SITTER_TYPESCRIPT_ROOT}/bindings/c/tree-sitter-${name}.pc.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/tree-sitter-${name}.pc" @ONLY)

    install(DIRECTORY "${TREE_SITTER_TYPESCRIPT_ROOT}/bindings/c/tree_sitter"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
            FILES_MATCHING PATTERN "*.h")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tree-sitter-${name}.pc"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig")
    install(TARGETS tree-sitter-${name}
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endmacro()

add_subdirectory(typescript tree-sitter-typescript)
