project(meshcleaner)
cmake_minimum_required(VERSION 2.8)

# Set these to configure what flavor of boost you want to find
# Multithreaded is required
set(Boost_USE_MULTITHREADED, ON)
# Static or dynamic linking of boost
set(Boost_USE_STATIC_LIBS, OFF)
set(Boost_USE_STATIC_RUNTIME, OFF)
find_package(Boost 1.50 COMPONENTS filesystem thread system REQUIRED)

aux_source_directory(. SRC_LIST)
aux_source_directory(lib_json LIBJSON_SRC_LIST)

include_directories(.)
include_directories(vcglib)

add_library(json ${LIBJSON_SRC_LIST})

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} json ${Boost_LIBRARIES})

