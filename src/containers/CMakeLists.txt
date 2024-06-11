set(HEADERS 
        basket.h 
        fill.h 
        order.h 
        route.h
)

add_custom_target(Containers ALL
    COMMENT "Copying Containers headers")

foreach(hdr ${HEADERS})
    add_custom_command(
        TARGET Containers
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${hdr} ${CMAKE_BINARY_DIR}/include"
    )
endforeach(hdr) 

add_dependencies(omslib Containers)
