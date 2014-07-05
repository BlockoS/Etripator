cmake_minimum_required (VERSION 2.8)

project(Etripator)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE} ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE} ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE} ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

if(MSVC)
    add_definitions (/D "_CRT_SECURE_NO_WARNINGS")
endif(MSVC)

find_package(Lua52)
find_package(Jansson)

set(Etripator_VERSION 1.0)
set(PROJECT_NAME Etripator)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})

include_directories( ${LUA_INCLUDE_DIR}
                     ${JANSSON_INCLUDE_DIR}
                     . )

set(ETRIPATOR_SOURCES
    etripator.cpp)

add_executable(etripator ${ETRIPATOR_SOURCES})
target_link_libraries(etripator ${LUA_LIBRARY} 
                                ${JANSSON_LIBRARY} )
