cmake_minimum_required(VERSION 3.7)
project(HexapodFirmware)

set(CMAKE_CXX_STANDARD 11)

add_executable(HexapodFirmware main.cpp AbstractBehavior.cpp Behavior.cpp CatBehavior.cpp Config.cpp Hexapod.cpp Joint.cpp Limb.cpp Serial.cpp utils.cpp SquareTestBehavior.cpp SquareTestBehavior.h LidarAPI.cpp LidarAPI.h LidarBehavior.cpp LidarBehavior.h)