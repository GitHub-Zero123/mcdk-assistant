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

add_subdirectory(libs/cpp-mcp EXCLUDE_FROM_ALL)
target_link_libraries(mcp PUBLIC nlohmann_json::nlohmann_json)

add_subdirectory(libs/tree-sitter EXCLUDE_FROM_ALL)
add_subdirectory(libs/tree-sitter-python EXCLUDE_FROM_ALL)
add_subdirectory(libs/tree-sitter-typescript EXCLUDE_FROM_ALL)

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
    src/sapi/sapi_index.cpp
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

    # 解决方案层是"查询/文档类"只读增强（非敏感文件操作），full/lite/server 各变体都应具备。
    # MCDK_WITH_SOLUTIONS 在函数被调用时已求值（检测块位于首个 mcdk_add_target 调用之前）。
    if(MCDK_WITH_SOLUTIONS)
        target_link_libraries(${target_name} PRIVATE mcdk_solutions)
        target_compile_definitions(${target_name} PRIVATE MCDK_WITH_SOLUTIONS)
    endif()

    mcdk_apply_platform_options(${target_name})
endfunction()

# ── 解决方案层（mcdk-solutions 子模块，可选）──
# 向下兼容：无子模块权限时，solutions/ 为空目录 —— 此时整体禁用相关功能，
# 主工程照常编译。以标记源文件是否存在判定「内容非空且含 C++ 部分」。
set(MCDK_WITH_SOLUTIONS OFF)
set(MCDK_SOLUTIONS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/solutions")
file(GLOB _mcdk_solutions_contents "${MCDK_SOLUTIONS_DIR}/*")
if(_mcdk_solutions_contents AND EXISTS "${MCDK_SOLUTIONS_DIR}/cpp/solution_index.cpp")
    set(MCDK_WITH_SOLUTIONS ON)
endif()

if(MCDK_WITH_SOLUTIONS)
    message(STATUS "[MCDK] solutions submodule detected — enabling --solution support")
    # 必须在任何链接 mcdk_solutions 的目标之前（下方 mcdk-assistant、以及末尾 add_subdirectory(tools)
    # 里的 mcdk-index-compiler）声明该库目标；勿把本行移到那些目标之后。
    add_subdirectory(solutions EXCLUDE_FROM_ALL)
else()
    message(STATUS "[MCDK] solutions submodule absent/empty — building without --solution support")
endif()

mcdk_add_target(mcdk-assistant
    WITH_STB
    SOURCES ${MCDK_SOURCES}
)

# LITE版最简分发（不提供任何涉及文件操作的敏感行为内容；但查询类的解决方案层仍具备）
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

set(MCDK_INSTALL_RUNTIME_TARGETS mcdk-assistant mcdk-asst-lite)
if(NOT ANDROID)
    list(APPEND MCDK_INSTALL_RUNTIME_TARGETS mcdk-asst-server)
endif()
install(TARGETS ${MCDK_INSTALL_RUNTIME_TARGETS} RUNTIME DESTINATION .)

install(DIRECTORY dicts/ DESTINATION dicts)

set(MCDK_INSTALL_CACHE_BINS
    "${CMAKE_CURRENT_BINARY_DIR}/mcdk_index_cache.bin"
    "${CMAKE_CURRENT_BINARY_DIR}/mcdk_sapi_index_cache.bin"
)
if(MCDK_WITH_SOLUTIONS)
    list(APPEND MCDK_INSTALL_CACHE_BINS
        "${CMAKE_CURRENT_BINARY_DIR}/mcdk_solutions_cache.bin"
    )
endif()
option(MCDK_INSTALL_TOOLS "Install helper tool executables" OFF)
install(FILES ${MCDK_INSTALL_CACHE_BINS} DESTINATION . OPTIONAL)

add_subdirectory(tools)

option(MCDK_BUILD_TESTS "Build test executables" OFF)
if(MCDK_BUILD_TESTS)
    add_subdirectory(tests)
endif()
