cmake_minimum_required(VERSION 3.10)
project(MiyooInputEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2 and SDL2_ttf
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

add_executable(MiyooInputEngine
    src/main.cpp
    src/InputEngine.cpp
)

target_link_libraries(MiyooInputEngine
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)
