cmake_minimum_required(VERSION 3.7)
project(raycaster)

set(CMAKE_CXX_STANDARD 11)
include_directories(include)
set(SOURCE_FILES
        include/CGame.h
        include/CGameSnapshot.cpp
        include/CGameSnapshot.h
        include/Common.h
        include/CRenderer.h
        SDLVersion/CSDLRenderer.cpp
        SDLVersion/main.cpp
        src/CGame.cpp)

add_executable(raycaster ${SOURCE_FILES})