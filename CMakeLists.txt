cmake_minimum_required(VERSION 3.20)
project(omscompare VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE RelDebugInfo)

find_package(Boost 1.85.0 REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(src/omstypes)
add_subdirectory(src/omsb)
add_subdirectory(src/omslite)
add_subdirectory(app)
#file(GLOB_RECURSE sources src/*cpp src/*.h)
#target_include_directories(example PUBLIC src/)

# add_executable(example ${sources})
