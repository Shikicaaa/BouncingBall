cmake_minimum_required(VERSION 3.10)

project(BouncingBall)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src/include)
link_directories(src/lib)

# Izvorni fajlovi
set(SOURCES
    WinMain.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(main ${SOURCES})

target_link_libraries(main
    mingw32
    SDL2main
    SDL2
    SDL2_ttf
    m
    stdc++
)
