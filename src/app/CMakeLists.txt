
add_executable(omscompare main.cpp)
add_library(omslib /dev/null)
target_sources(omslib PRIVATE
    client_state.cpp 
    client.cpp
    db.cpp 
    idtype.cpp 
    metrics.cpp 
    uuid.cpp 
    workflows.cpp
)

#target_include_directories(omslib PUBLIC .)

target_link_libraries(omscompare PUBLIC omslib Boost)
target_include_directories(omslib PUBLIC ${CMAKE_SOURCE_DIR}/src/include)

target_link_libraries(omscompare PUBLIC omslib Boost)
