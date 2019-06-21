cmake_minimum_required (VERSION 2.8)
project (pipelog)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(PkgConfig REQUIRED)
pkg_search_module(USB_LIB REQUIRED libusb-1.0)

link_directories(${CMAKE_PREFIX_PATH}/lib/lib/64bit)

add_executable(pipelog main.cpp)
target_link_libraries(pipelog ${USB_LIB} hid  powerusb dl)

install(TARGETS pipelog DESTINATION bin)
