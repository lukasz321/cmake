cmake_minimum_required (VERSION 2.8)
project (pipelog)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(POWERSTRIP_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/"src/lib/64bit")
add_subdirectory(src)
