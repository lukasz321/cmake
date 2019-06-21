add_executable(pipelog main.cpp PwrUSBHid.c PwrUSBImp.cpp)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

#find_package(libusb-1.0-0 REQUIRED)
#find_package(PkgConfig REQUIRED)
#pkg_search_module(LIBUSB1 REQUIRED libusb-1.0)
#include_directories(SYSTEM ${LIBUSB1_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_search_module(USB_LIB REQUIRED libusb-1.0)

target_link_libraries(pipelog usb-1.0 ${CMAKE_DL_LIBS})

install(TARGETS pipelog DESTINATION bin)
