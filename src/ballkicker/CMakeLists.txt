cmake_minimum_required(VERSION 3.7)

# set the project name
project(Ballkicker C)
set(EV3_LIB ../ev3dev-c/source/ev3)
set(SOURCES   ${EV3_LIB}/*.h)

include_directories(${EV3_LIB})
# add the executable
add_executable(ballkicker main.c beep.h beep.c)
set_property(TARGET ballkicker PROPERTY C_STANDARD 99)
target_link_libraries(ballkicker ev3dev-c)
