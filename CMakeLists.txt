cmake_minimum_required(VERSION 3.5)

project(minesweeper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point this to your Qt installation
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.3/mingw_64/lib/cmake")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(minesweeper qt.cpp)

target_link_libraries(minesweeper PRIVATE Qt6::Widgets)
