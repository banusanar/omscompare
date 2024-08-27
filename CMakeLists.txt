cmake_minimum_required(VERSION 3.28)
project(omscompare VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

#string(APPEND CMAKE_CXX_FLAGS " -Wall")
#string(APPEND CMAKE_CXX_FLAGS " -Wbuiltin-macro-redefined")
#string(APPEND CMAKE_CXX_FLAGS " -pedantic")
#string(APPEND CMAKE_CXX_FLAGS " -Werror")

find_package(Boost 1.85.0 REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})

find_package(SQLiteCpp REQUIRED)
add_subdirectory(src/app)
