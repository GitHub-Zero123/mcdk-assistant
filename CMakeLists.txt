cmake_minimum_required(VERSION 3.15)

project(mcdk-assistant LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# By Zero123
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    if(MSVC)
        add_compile_options(/utf-8 /EHsc)
        # 使用MT链接运行库
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

# ── 依赖 ──────────────────────────────────────────────

# nlohmann/json (本地单头文件)
add_library(nlohmann_json INTERFACE)
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
target_include_directories(nlohmann_json INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann
)

# Threads
find_package(Threads REQUIRED)

# cpp-mcp
add_subdirectory(libs/cpp-mcp)
target_link_libraries(mcp PUBLIC nlohmann_json::nlohmann_json)

# cppjieba (header-only, 手动设置 include)
add_library(cppjieba INTERFACE)
target_include_directories(cppjieba INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cppjieba/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cppjieba/deps/limonp/include
)

# stb (header-only, 像素画模块依赖)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb
)

# ── 主程序 ────────────────────────────────────────────

file(GLOB_RECURSE MCDK_SOURCES
    src/*.cpp
)

add_executable(${PROJECT_NAME} ${MCDK_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    mcp
    cppjieba
    stb
    nlohmann_json::nlohmann_json
    Threads::Threads
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

# ── 安装 ──────────────────────────────────────────────

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY dicts/ DESTINATION dicts)
install(DIRECTORY knowledge/ DESTINATION knowledge)

# ── 测试 ──────────────────────────────────────────────

option(MCDK_BUILD_TESTS "Build test executables" OFF)
if(MCDK_BUILD_TESTS)
    add_subdirectory(tests)
endif()
