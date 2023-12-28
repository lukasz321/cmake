cmake_minimum_required(VERSION 3.7.2)

project(powerusb)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)

find_package(PkgConfig)

find_package(ZLIB REQUIRED)

pkg_check_modules ( PKGCONFIG_LIBUSB libusb-1.0 )
IF(NOT PKGCONFIG_LIBUSB_FOUND)
    message (FATAL_ERROR "Cannot find libusb")
endif()

include_directories(
    ${PKGCONFIG_LIBUSB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/installed/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/powerstrip/
    )

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/installed/lib/
    ${PKGCONFIG_LIBUSB_LIBRARIES}
    )

add_executable(powerusb 
    src/main.cpp 
    src/powerstrip/PwrUSBHid.c
    src/powerstrip/PwrUSBImp.cpp
    )

add_executable(on 
    src/on.cpp 
    src/powerstrip/PwrUSBHid.c
    src/powerstrip/PwrUSBImp.cpp
    )

add_executable(off 
    src/off.cpp 
    src/powerstrip/PwrUSBHid.c
    src/powerstrip/PwrUSBImp.cpp
    )

target_include_directories(powerusb PRIVATE ${ZLIB_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 11)

target_link_libraries(powerusb
    ${PKGCONFIG_LIBUSB_LIBRARIES}
    ${ZLIB_LIBRARIES} 
    )

target_link_libraries(on
    ${PKGCONFIG_LIBUSB_LIBRARIES}
    ${ZLIB_LIBRARIES} 
    )

target_link_libraries(off
    ${PKGCONFIG_LIBUSB_LIBRARIES}
    ${ZLIB_LIBRARIES} 
    )
