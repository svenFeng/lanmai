cmake_minimum_required(VERSION 3.10)

project(lanmai)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
file(GLOB LANMAI_CPPFILES src/*.cpp)
add_executable(lanmai ${LANMAI_CPPFILES})

target_include_directories(lanmai PUBLIC ./lib /usr/include/libevdev-1.0)
target_link_libraries(lanmai PUBLIC udev evdev)

install(TARGETS lanmai DESTINATION /usr/bin)
install(CODE 
    "IF(NOT EXISTS /etc/lanmai.json)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/etc/lanmai.json DESTINATION /etc)
    ENDIF()")

install(CODE 
    "IF(NOT EXISTS /usr/lib/systemd/system/lanmai.service)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/etc/lanmai.service DESTINATION /usr/lib/systemd/system)
    ENDIF()")
