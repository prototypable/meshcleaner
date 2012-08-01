project(meshcleaner)
cmake_minimum_required(VERSION 2.8)

option(BUILD_REST_SERVER "Build the REST server" OFF)

set(MESHCLEANER_SOURCES
    meshcleanerworker.hpp           meshcleanerworker.cpp
    mesh_definitions.hpp            mesh_definitions.cpp
    meshcleaner.cpp
)

set(REST_SERVER_SOURCES
    meshcleanerworker.hpp           meshcleanerworker.cpp
    mesh_definitions.hpp            mesh_definitions.cpp
    http_server.hpp                 http_server.cpp
    meshcleaner_rest_server.cpp
)

set(STLCONVERT_SOURCES
    stlconvert.cpp
)

set(MANIFOLDTEST_SOURCES
    manifoldtest.cpp
)

if(BUILD_REST_SERVER)
    # Set these to configure what flavor of boost you want to find
    # Multithreaded is required
    set(Boost_USE_MULTITHREADED, ON)
    # Static or dynamic linking of boost
    set(Boost_USE_STATIC_LIBS, OFF)
    set(Boost_USE_STATIC_RUNTIME, OFF)
    find_package(Boost 1.50 COMPONENTS filesystem thread system REQUIRED)

    aux_source_directory(lib_json LIBJSON_SRC_LIST)

    add_library(json ${LIBJSON_SRC_LIST})
endif(BUILD_REST_SERVER)

include_directories(.)
include_directories(vcglib)

if (BUILD_REST_SERVER)
    add_executable(meshcleaner_rest_server ${REST_SERVER_SOURCES})
    target_link_libraries(meshcleaner_rest_server json ${Boost_LIBRARIES})
endif(BUILD_REST_SERVER)

add_executable(meshcleaner ${MESHCLEANER_SOURCES})
add_executable(stlconvert ${STLCONVERT_SOURCES})
add_executable(manifoldtest ${MANIFOLDTEST_SOURCES})
