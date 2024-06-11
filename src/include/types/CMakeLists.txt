
set(HEADERS basket.h fill.h order.h route.h)

add_custom_command(OUTPUT mkdirtypes
    COMMAND ${CMAKE_COMMAND} -E make_directory  "${CMAKE_BINARY_DIR}/include/types/"
)

add_custom_target(Types ALL
    COMMENT "Copying Types headers")

foreach(hdr ${HEADERS})
    add_custom_command(
        TARGET Types
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${hdr} ${CMAKE_BINARY_DIR}/include/types"
    )
endforeach(hdr) 

target_sources(omslib PRIVATE 
    idtype.cpp
    uuid.cpp
)
add_dependencies(omslib Types)


