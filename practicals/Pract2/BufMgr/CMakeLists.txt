cmake_minimum_required (VERSION 2.4)
project (minibase)

set(CMAKE_CXX_FLGAS "-Wall -O0 -g")
set(CMAKE_BUILD_TYPE Debug)

find_library(SPACEMGR_LIB spacemgr lib/)
find_library(BTREE_LIB btree lib/)
find_library(GLOBALDEFS_LIB globaldefs lib/)
find_library(JOINS_LIB joins lib/)

add_subdirectory(bufmgr)

add_executable (minibase-bufmgr main.cpp test.cpp)
target_link_libraries (minibase-bufmgr ${JOINS_LIB} ${BTREE_LIB} ${SPACEMGR_LIB} bufmgr ${SPACEMGR_LIB} ${GLOBALDEFS_LIB} ${SPACEMGR_LIB}) 
