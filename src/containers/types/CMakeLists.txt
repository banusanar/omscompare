
add_library(Types INTERFACE)

target_sources(Types PUBLIC 
    FILE_SET publicheaders 
    TYPE HEADERS FILES
    basket.h 
    fill.h 
    order.h 
    route.h 
) 

install(TARGETS Types FILE_SET publicheaders)
