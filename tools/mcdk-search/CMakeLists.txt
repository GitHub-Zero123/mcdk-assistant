include(FetchContent)
set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

add_executable(mcdk-search
    main.cpp
)

target_include_directories(mcdk-search PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(mcdk-search PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
    cppjieba
    zstd
    Threads::Threads
)

if(WIN32)
    target_link_libraries(mcdk-search PRIVATE ws2_32)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    target_compile_options(mcdk-search PRIVATE
        -Wno-deprecated-declarations
        -Wno-unused-result
    )
    target_link_options(mcdk-search PRIVATE
        -static-libstdc++
        -static-libgcc
        -Wl,-Bstatic -lm
        -Wl,-Bdynamic
    )
endif()

if(MCDK_INSTALL_TOOLS)
    install(TARGETS mcdk-search RUNTIME DESTINATION .)
endif()
