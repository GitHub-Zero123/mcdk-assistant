add_executable(mcdk-index-compiler
    index_compiler_main.cpp
    ${CMAKE_SOURCE_DIR}/src/app/runtime_paths.cpp
    ${CMAKE_SOURCE_DIR}/src/app/server_runtime.cpp
    ${CMAKE_SOURCE_DIR}/src/sapi/sapi_index.cpp
)

target_include_directories(mcdk-index-compiler PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(mcdk-index-compiler PRIVATE
    mcp
    cppjieba
    zstd
    tree-sitter
    tree-sitter-typescript
    nlohmann_json::nlohmann_json
    Threads::Threads
)

target_compile_definitions(mcdk-index-compiler PRIVATE MCDK_LITE MCDK_INDEX_COMPILER)

# 解决方案层可用时，让索引编译器一并产出 mcdk_solutions_cache.bin（与 SAPI 缓存同处理）。
if(MCDK_WITH_SOLUTIONS)
    target_link_libraries(mcdk-index-compiler PRIVATE mcdk_solutions)
    target_compile_definitions(mcdk-index-compiler PRIVATE
        MCDK_WITH_SOLUTIONS
        MCDK_SOLUTIONS_CONTENT_DIR="${CMAKE_SOURCE_DIR}/solutions/solutions"
    )
endif()

if(WIN32)
    target_link_libraries(mcdk-index-compiler PRIVATE ws2_32)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    target_compile_options(mcdk-index-compiler PRIVATE
        -Wno-deprecated-declarations
        -Wno-unused-result
    )
    target_link_options(mcdk-index-compiler PRIVATE
        -static-libstdc++
        -static-libgcc
        -Wl,-Bstatic -lm
        -Wl,-Bdynamic
    )
endif()

if(MCDK_INSTALL_TOOLS)
    install(TARGETS mcdk-index-compiler RUNTIME DESTINATION .)
endif()
