add_executable(mcdk-python-review
    main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/python_review/review_analyzer.cpp
)

target_include_directories(mcdk-python-review PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

target_compile_features(mcdk-python-review PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(mcdk-python-review PRIVATE /utf-8 /EHsc)
endif()

target_link_libraries(mcdk-python-review PRIVATE
    tree-sitter
    tree-sitter-python
    nlohmann_json::nlohmann_json
)

if(WIN32)
    target_link_libraries(mcdk-python-review PRIVATE Shell32)
endif()
