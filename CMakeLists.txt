cmake_minimum_required (VERSION 2.8)

project(Etripator)

# Force build type to Debug if none was specified.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(LIBRARY_OUTPUT_PATH    "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(ARCHIVE_OUTPUT_PATH    "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

find_package(Lua52)
find_package(Jansson)
find_package(UnitTest++)

set(Etripator_VERSION 1.0)
set(PROJECT_NAME Etripator)

include_directories( ${LUA_INCLUDE_DIR}
                     ${JANSSON_INCLUDE_DIR}
                     include/ )

set( Etripator_SRC
  src/memory.cpp
  src/parser/utils.cpp )
add_library( etripator STATIC ${Etripator_SRC} )

if(BUILD_TESTS)
  find_package(UnitTest++)
  include_directories( ${UNITTEST++_INCLUDE_DIR} )
  set( Etripator_TEST_SRC
       src/test/RunTests.cpp
       src/test/memory.t.cpp
       src/test/parser/utils.t.cpp )
  add_executable( RunTests ${Etripator_TEST_SRC} )
  target_link_libraries( RunTests etripator )
  target_link_libraries( RunTests ${UNITTEST++_LIBRARY} )
endif()