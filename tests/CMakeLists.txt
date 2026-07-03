cmake_minimum_required(VERSION 3.15)
project(mcdk-tests LANGUAGES CXX)

add_executable(search_test search_test.cpp)

add_executable(python_analysis_test
    python_analysis_test.cpp
    ../src/project_analysis/project_analyzer.cpp
)

add_executable(sapi_index_test
    sapi_index_test.cpp
    ../src/sapi/sapi_index.cpp
)

target_include_directories(search_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_include_directories(python_analysis_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_include_directories(sapi_index_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_compile_features(search_test PRIVATE cxx_std_20)
target_compile_features(python_analysis_test PRIVATE cxx_std_20)
target_compile_features(sapi_index_test PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(search_test PRIVATE /utf-8 /EHsc)
    target_compile_options(python_analysis_test PRIVATE /utf-8 /EHsc)
    target_compile_options(sapi_index_test PRIVATE /utf-8 /EHsc)
endif()

target_link_libraries(search_test PRIVATE
    cppjieba
    nlohmann_json::nlohmann_json
)

target_link_libraries(python_analysis_test PRIVATE
    tree-sitter
    tree-sitter-python
)

target_link_libraries(sapi_index_test PRIVATE
    tree-sitter
    tree-sitter-typescript
)

target_compile_definitions(search_test PRIVATE
    MCDK_KNOWLEDGE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../knowledge"
    MCDK_DICTS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../dicts"
)

target_compile_definitions(sapi_index_test PRIVATE
    MCDK_WORKSPACE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/.."
)

# NBT 编解码自检（仅依赖 nlohmann，夹具用真实 .mcstructure）
add_executable(nbt_test nbt_test.cpp)
target_include_directories(nbt_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/cpp-mcp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/cpp-mcp/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/cpp-mcp/common
)
target_compile_features(nbt_test PRIVATE cxx_std_20)
if(MSVC)
    target_compile_options(nbt_test PRIVATE /utf-8 /EHsc)
endif()
target_link_libraries(nbt_test PRIVATE nlohmann_json::nlohmann_json)
target_compile_definitions(nbt_test PRIVATE
    MCDK_KNOWLEDGE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../knowledge"
)

if(TARGET mcdk_pocketpy_bridge)
    add_executable(plugin_fs_smoke_test plugin_fs_smoke_test.cpp)
    target_include_directories(plugin_fs_smoke_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
    )
    target_compile_features(plugin_fs_smoke_test PRIVATE cxx_std_20)
    if(MSVC)
        target_compile_options(plugin_fs_smoke_test PRIVATE /utf-8 /EHsc)
    endif()
    target_link_libraries(plugin_fs_smoke_test PRIVATE mcdk_pocketpy_bridge)
    if(WIN32)
        target_link_libraries(plugin_fs_smoke_test PRIVATE ws2_32)
    endif()
endif()
