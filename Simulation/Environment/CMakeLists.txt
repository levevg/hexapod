cmake_minimum_required(VERSION 3.7)
project(LidarHexapod)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Camera.cpp Camera.h Utils.cpp Utils.h FPSCounter.cpp FPSCounter.h CommandLine.cpp CommandLine.h Point3D.cpp Point3D.h VirtualRoom.cpp VirtualRoom.h Point2D.cpp Point2D.h Segment2D.cpp Segment2D.h Pod.cpp Pod.h FirmwareCode/Config.cpp FirmwareCode/Hexapod.cpp FirmwareCode/Serial.cpp FirmwareCode/Joint.cpp FirmwareCode/Limb.cpp FirmwareCode/AbstractBehavior.cpp ComReciever.h ComReciever.cpp Lidar.cpp Lidar.h)
add_executable(LidarHexapod ${SOURCE_FILES})

include_directories("c:/Program Files/WinBiulds/mingw/freeglut/include")
#include_directories("c:/MinGW/freeglut/include")

target_link_libraries(LidarHexapod "c:/Program Files/WinBiulds/mingw/freeglut/lib/libfreeglut.dll.a" opengl32 glu32 -Wl,--subsystem,windows)
#target_link_libraries(LidarHexapod "c:/MinGW/freeglut/lib/libfreeglut.dll.a" opengl32 glu32 -Wl,--subsystem,windows)