
add_library(Sqlite INTERFACE
    db.h 
)

install(TARGETS Sqlite EXPORT SqliteTargets 
    INCLUDES DESTINATION include
)

install(EXPORT SqliteTargets
    FILE SqliteTargets.cmake 
    NAMESPACE Omscompare::
    DESTINATION lib/cmake/omscompare
)