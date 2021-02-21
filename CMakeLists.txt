cmake_minimum_required(VERSION 3.0)

project(usb-sniffily)

find_package(PkgConfig)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
include_directories( ${LIBUSB_INCLUDE_DIRS} )

find_package(Threads REQUIRED)

include_directories( include )

add_library(sniffify	src/raw-helper.c
			include/raw-helper.h)

add_executable(passthrough src/passthrough.c)
add_executable(libusbhax src/libusbhax.c)
add_executable(hid src/hid.c)

target_link_libraries(passthrough sniffify ${LIBUSB_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} m)
target_link_libraries(libusbhax ${LIBUSB_LIBRARIES} )
target_link_libraries(hid sniffify ${LIBUSB_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})