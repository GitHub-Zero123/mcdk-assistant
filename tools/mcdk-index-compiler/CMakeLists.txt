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
    tree-sitter
    tree-sitter-typescript
    nlohmann_json::nlohmann_json
    Threads::Threads
)

target_compile_definitions(mcdk-index-compiler PRIVATE MCDK_LITE MCDK_INDEX_COMPILER)

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

install(TARGETS mcdk-index-compiler RUNTIME DESTINATION .)
