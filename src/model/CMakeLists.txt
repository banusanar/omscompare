set(HEADERS 
        client_state.h 
        metrics.h )

add_custom_target(ModelHeaders ALL
    COMMENT "Copying ModelHeaders headers")

foreach(hdr ${HEADERS})
    add_custom_command(
        TARGET ModelHeaders
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${hdr} ${CMAKE_BINARY_DIR}/include"
    )
endforeach(hdr) 

add_dependencies(omslib ModelHeaders)

target_sources(omslib PRIVATE 
    client_state.cpp
    metrics.cpp
)

find_package(Boost 1.85.0 REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})
