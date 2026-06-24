cmake_minimum_required(VERSION 3.15)
project(mcdk-tests LANGUAGES CXX)

add_executable(search_test search_test.cpp)

add_executable(python_analysis_test
    python_analysis_test.cpp
    ../src/project_analysis/project_analyzer.cpp
)

target_include_directories(search_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_include_directories(python_analysis_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_compile_features(search_test PRIVATE cxx_std_20)
target_compile_features(python_analysis_test PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(search_test PRIVATE /utf-8 /EHsc)
    target_compile_options(python_analysis_test PRIVATE /utf-8 /EHsc)
endif()

target_link_libraries(search_test PRIVATE
    cppjieba
    nlohmann_json::nlohmann_json
)

target_link_libraries(python_analysis_test PRIVATE
    tree-sitter
    tree-sitter-python
)

target_compile_definitions(search_test PRIVATE
    MCDK_KNOWLEDGE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../knowledge"
    MCDK_DICTS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../dicts"
)

# NBT 编解码自检（仅依赖 nlohmann，夹具用真实 .mcstructure）
add_executable(nbt_test nbt_test.cpp)
target_include_directories(nbt_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_compile_features(nbt_test PRIVATE cxx_std_20)
if(MSVC)
    target_compile_options(nbt_test PRIVATE /utf-8 /EHsc)
endif()
target_link_libraries(nbt_test PRIVATE nlohmann_json::nlohmann_json)
target_compile_definitions(nbt_test PRIVATE
    MCDK_KNOWLEDGE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../knowledge"
)
