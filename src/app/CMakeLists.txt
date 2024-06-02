
add_library(App STATIC)

target_sources(App PRIVATE
    workflows.cpp
    client.cpp
)

target_sources(App 
    PUBLIC 
    FILE_SET publicheaders 
    TYPE HEADERS FILES
    workflows.h
    client.h
)

target_link_libraries(App PUBLIC Model Containers Sqlite)

install(TARGETS App FILE_SET publicheaders)