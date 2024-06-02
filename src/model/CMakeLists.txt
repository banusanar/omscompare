
add_library(Model STATIC)

target_sources(Model PRIVATE 
    client_state.cpp
)

target_sources(Model 
    PUBLIC 
    FILE_SET publicheaders 
    TYPE HEADERS FILES
    client_state.h
)

find_package(Boost 1.85.0 REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(Model PUBLIC Boost Containers Sqlite)

install(TARGETS Model FILE_SET publicheaders)