cmake_minimum_required(VERSION 3.15)
project(Flir)

set(CMAKE_CXX_STANDARD 14)

add_executable(Flir main.cpp FlirCamera.cpp FlirCamera.h)
include_directories(/usr/local/include/spinnaker/)
target_link_libraries(Flir PUBLIC /usr/local/lib/libSpinnaker.dylib)