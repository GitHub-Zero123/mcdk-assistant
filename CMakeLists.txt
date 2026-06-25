cmake_minimum_required(VERSION 3.15)

project(MCDK-ASSISTANT LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# By Zero123
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    if(MSVC)
        add_compile_options(/utf-8 /EHsc)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

# 内部依赖库
add_library(nlohmann_json INTERFACE)
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
target_include_directories(nlohmann_json INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann
)

find_package(Threads REQUIRED)

add_subdirectory(libs/cpp-mcp)
target_link_libraries(mcp PUBLIC nlohmann_json::nlohmann_json)

add_subdirectory(libs/tree-sitter)
add_subdirectory(libs/tree-sitter-python)
add_subdirectory(libs/tree-sitter-typescript)

add_library(cppjieba INTERFACE)
target_include_directories(cppjieba INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cppjieba/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cppjieba/deps/limonp/include
)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb
)

file(GLOB_RECURSE MCDK_SOURCES
    src/*.cpp
)

set(MCDK_LITE_SOURCES
    src/main.cpp
    src/app/runtime_paths.cpp
    src/app/server_runtime.cpp
    src/project_analysis/project_analyzer.cpp
)

# 平台处理函数封装
function(mcdk_apply_platform_options target_name)
    if(WIN32)
        target_link_libraries(${target_name} PRIVATE ws2_32)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
        target_compile_options(${target_name} PRIVATE
            -Wno-deprecated-declarations
            -Wno-unused-result
        )
        target_link_options(${target_name} PRIVATE
            -static-libstdc++
            -static-libgcc
            -Wl,-Bstatic -lm
            -Wl,-Bdynamic
        )
    endif()
endfunction()

# 添加目标函数封装
function(mcdk_add_target target_name)
    set(options WITH_STB)
    set(oneValueArgs)
    set(multiValueArgs SOURCES DEFINITIONS)
    cmake_parse_arguments(MCDK_TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${target_name} ${MCDK_TARGET_SOURCES})
    target_include_directories(${target_name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    target_link_libraries(${target_name} PRIVATE
        mcp
        cppjieba
        tree-sitter
        tree-sitter-python
        tree-sitter-typescript
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    if(MCDK_TARGET_WITH_STB)
        target_link_libraries(${target_name} PRIVATE stb)
    endif()

    if(MCDK_TARGET_DEFINITIONS)
        target_compile_definitions(${target_name} PRIVATE ${MCDK_TARGET_DEFINITIONS})
    endif()

    mcdk_apply_platform_options(${target_name})
endfunction()

mcdk_add_target(mcdk-assistant
    WITH_STB
    SOURCES ${MCDK_SOURCES}
)

# LITE版最简分发（不提供任何涉及文件操作的敏感行为内容）
mcdk_add_target(mcdk-asst-lite
    SOURCES ${MCDK_LITE_SOURCES}
    DEFINITIONS MCDK_LITE
)

if(NOT ANDROID)
    file(GLOB_RECURSE MCDK_SERVER_SOURCES src/*.cpp)
    # 服务器默认LITE模式，避免暴露过多文件操作接口
    mcdk_add_target(mcdk-asst-server
        SOURCES ${MCDK_SERVER_SOURCES}
        DEFINITIONS MCDK_SERVER MCDK_LITE
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    install(TARGETS mcdk-asst-lite mcdk-asst-lite mcdk-asst-server RUNTIME DESTINATION .)
else()
    install(TARGETS mcdk-asst-lite mcdk-asst-lite RUNTIME DESTINATION .)
endif()
install(DIRECTORY dicts/ DESTINATION dicts)
install(DIRECTORY knowledge/ DESTINATION knowledge)

add_subdirectory(tools)

option(MCDK_BUILD_TESTS "Build test executables" OFF)
if(MCDK_BUILD_TESTS)
    add_subdirectory(tests)
endif()
