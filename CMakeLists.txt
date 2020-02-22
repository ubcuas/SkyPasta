cmake_minimum_required(VERSION 3.15)
project(skypasta)

set(CMAKE_CXX_STANDARD 14)


add_executable(skypasta main.cpp FlirCamera.cpp FlirCamera.h ImageRetriever.cpp ImageRetriever.h Telemetry.cpp Telemetry.h Telemetry.cpp Telemetry.h ImageTag.cpp ImageTag.h lib/json.hpp)

include_directories(/usr/local/include/spinnaker/)
include_directories(/usr/local/include/exiv2/)
target_link_libraries(skypasta PUBLIC /usr/local/lib/libSpinnaker.dylib)
target_link_libraries(skypasta PUBLIC /usr/local/lib/libexiv2.26.dylib)