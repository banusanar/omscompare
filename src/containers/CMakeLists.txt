
add_library(Containers INTERFACE)

target_sources(Containers PUBLIC 
    FILE_SET publicheaders 
    TYPE HEADERS FILES
    basket.h 
    fill.h 
    order.h 
    route.h 
) 

target_link_libraries(Containers INTERFACE Types)
install(TARGETS Containers FILE_SET publicheaders)
