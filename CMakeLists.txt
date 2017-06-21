cmake_minimum_required(VERSION 3.7)
project(raycaster)

find_package(SDL REQUIRED)

set(CMAKE_CXX_STANDARD 11)
include_directories(include)
include_directories(noudar-core/noudar-core/include)
include_directories(stb)

set(SOURCE_FILES
        include/CGame.h
        include/CGameSnapshot.cpp
        include/CGameSnapshot.h
        include/Common.h
        include/CRenderer.h
        SDLVersion/CSDLRenderer.cpp
        SDLVersion/main.cpp
        src/LoadPNG.cpp
        src/NativeBitmap.cpp
        noudar-core/noudar-core/src/CPlainFileLoader.cpp
        noudar-core/noudar-core/src/Common.cpp
        src/CGame.cpp)

find_package(OpenGL REQUIRED)

include (FindPkgConfig)

add_executable(raycaster ${SOURCE_FILES})

target_link_libraries(raycaster ${OPENGL_LIBRARIES} ${SDL_LIBRARY}   )