find_path( UNITTEST++_INCLUDE_DIR 
    NAMES
        unittest++/UnitTest++.h
    PATHS
        "${UNITTEST++_LOCATION}/include"
        "$ENV{UNITTEST++_LOCATION}/include"
        "$ENV{PROGRAMFILES}/GLFW/include"
        /usr/include/
        /usr/local/include
    DOC 
        "The directory where UnitTest++/UnitTest++.h resides"
)

find_library( UNITTEST++_LIBRARY 
    NAMES
        UnitTest++
    PATHS
        "${UNITTEST++_LOCATION}/lib"
        "${UNITTEST++_LOCATION}/lib/x64"
        "$ENV{UNITTEST++_LOCATION}/lib"
        "$ENV{UNITTEST++_LIBRARY}/lib/x64"
        /usr/lib
        /usr/lib64
        /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
        /usr/lib/w32api
        /usr/local/lib
        /usr/local/lib/${CMAKE_LIBRARY_ARCHITECTURE}
    DOC 
        "The UnitTest++ library"
)

set (UNITTEST++_FOUND "NO")

if(UNITTEST++_INCLUDE_DIR AND UNITTEST++_LIBRARY)
        set (UNITTEST++_FOUND "YES")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(UNITTEST++
    REQUIRED_VARS
        UNITTEST++_INCLUDE_DIR
        UNITTEST++_LIBRARY
)

mark_as_advanced(
    UNITTEST++_INCLUDE_DIR
    UNITTEST++_LIBRARY
)
