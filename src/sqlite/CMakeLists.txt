set(HEADERS 
    db.h)

add_custom_target(SqliteHeaders ALL
    COMMENT "Copying sqlite headers")

foreach(hdr ${HEADERS})
    add_custom_command(
        TARGET SqliteHeaders
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${hdr} ${CMAKE_BINARY_DIR}/include"
    )
endforeach(hdr) 

add_dependencies(omslib SqliteHeaders)

target_sources(omslib PRIVATE 
    db.cpp
)