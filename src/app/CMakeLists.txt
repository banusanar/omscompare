
add_executable(omscompare main.cpp)
add_library(omslib /dev/null)
target_sources(omslib PRIVATE
    client_state_boost.cpp 
    client_state_boost_hashed.cpp 
    client_state_template_btree.cpp 
    client_state_template_hashed.cpp 
    client_state_sqlite.cpp 
    client_state_sqlite_setup.cpp 
    client.cpp
    idtype.cpp 
    metrics.cpp 
    uuid.cpp 
    workflows.cpp
)

target_include_directories(omslib PUBLIC ${CMAKE_SOURCE_DIR}/src/include)

target_link_libraries(omscompare PUBLIC omslib Boost::boost)
target_link_libraries(omscompare PUBLIC SQLiteCpp)