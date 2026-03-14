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

# ── 主程序（完整版）─────────────────────────────────────

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

# ── Linux 编译警告抑制 ──────────────────────────────────
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wno-deprecated-declarations
        -Wno-unused-result
    )
endif()

# ── Linux 静态链接 libstdc++/libgcc/libm（保持 libc 动态以兼容 glibc）──
# 在 ECS 等生产环境部署时无需安装额外运行时，二进制可直接运行
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    target_link_options(${PROJECT_NAME} PRIVATE
        -static-libstdc++
        -static-libgcc
        # libm/libpthread 通过 -Wl 静态优先链接
        -Wl,-Bstatic -lm
        -Wl,-Bdynamic   # 后续 libc/ld 保持动态
    )
endif()

# ── Lite 版（仅资料库搜索 + 网易差异工具）────────────

set(MCDK_LITE_SOURCES
    src/main.cpp
)

add_executable(mcdk-asst-lite ${MCDK_LITE_SOURCES})

target_compile_definitions(mcdk-asst-lite PRIVATE MCDK_LITE)

target_include_directories(mcdk-asst-lite PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mcdk-asst-lite PRIVATE
    mcp
    cppjieba
    nlohmann_json::nlohmann_json
    Threads::Threads
)

if(WIN32)
    target_link_libraries(mcdk-asst-lite PRIVATE ws2_32)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    target_compile_options(mcdk-asst-lite PRIVATE
        -Wno-deprecated-declarations
        -Wno-unused-result
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    target_link_options(mcdk-asst-lite PRIVATE
        -static-libstdc++
        -static-libgcc
        -Wl,-Bstatic -lm
        -Wl,-Bdynamic
    )
endif()

# ── Server 版（Linux 服务器部署，启用统计功能）─────────
# 仅在 Linux 上构建，定义 MCDK_SERVER 宏
# 特性：
#   - 监听 0.0.0.0:18766（MCP）
#   - 独立 HTTP 端口 18767 暴露 GET /api/tool-stats（工具调用统计）
#   - 统计数据持久化到 <exe_dir>/datas/tool_stats.json，每 30 分钟写盘

if(NOT ANDROID)
    file(GLOB_RECURSE MCDK_SERVER_SOURCES src/*.cpp)

    add_executable(mcdk-asst-server ${MCDK_SERVER_SOURCES})

    # Server 版同时定义 MCDK_LITE，屏蔽像素画/模型/动画等需要本地文件系统的高风险工具
    target_compile_definitions(mcdk-asst-server PRIVATE MCDK_SERVER MCDK_LITE)

    target_include_directories(mcdk-asst-server PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    # Server 版同时定义 MCDK_LITE，不编译像素画/模型/动画模块，无需 stb
    target_link_libraries(mcdk-asst-server PRIVATE
        mcp
        cppjieba
        nlohmann_json::nlohmann_json
        Threads::Threads
    )

    if(WIN32)
        target_link_libraries(mcdk-asst-server PRIVATE ws2_32)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_compile_options(mcdk-asst-server PRIVATE
            -Wno-deprecated-declarations
            -Wno-unused-result
        )

        target_link_options(mcdk-asst-server PRIVATE
            -static-libstdc++
            -static-libgcc
            -Wl,-Bstatic -lm
            -Wl,-Bdynamic
        )
    endif()
endif()

# ── 安装 ──────────────────────────────────────────────

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    install(TARGETS ${PROJECT_NAME} mcdk-asst-lite mcdk-asst-server RUNTIME DESTINATION .)
else()
    install(TARGETS ${PROJECT_NAME} mcdk-asst-lite RUNTIME DESTINATION .)
endif()
install(DIRECTORY dicts/ DESTINATION dicts)
install(DIRECTORY knowledge/ DESTINATION knowledge)

# ── 测试 ──────────────────────────────────────────────

option(MCDK_BUILD_TESTS "Build test executables" OFF)
if(MCDK_BUILD_TESTS)
    add_subdirectory(tests)
endif()
