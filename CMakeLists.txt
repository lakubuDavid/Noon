cmake_minimum_required(VERSION 3.0)
project(noon)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -llua -lboost_filesystem -lboost_filesystem -lboost_chrono -lboost_thread -lssl -lcrypto  -g")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

set(BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(OBJ_DIR ${CMAKE_CURRENT_SOURCE_DIR}/obj)

file(MAKE_DIRECTORY ${BIN_DIR})
file(MAKE_DIRECTORY ${OBJ_DIR})

include_directories(${INC_DIR})

add_executable(${PROJECT_NAME} ${SRC_DIR}/ ${LIB_DIR}/)

target_link_libraries(${PROJECT_NAME} lua boost_filesystem boost_chrono boost_thread ssl crypto)