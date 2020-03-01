cmake_minimum_required(VERSION 3.15)
project(skypasta)

set(CMAKE_CXX_STANDARD 14)

#include(FetchContent)

#FetchContent_Declare(exiv2cmake
#        URL https://www.exiv2.org/builds/exiv2-0.27.2-Source.tar.gz)
#FetchContent_GetProperties(exiv2cmake)
#if (NOT exiv2cmake_POPULATED)
#    FetchContent_Populate(exiv2cmake)
#    endif()
#add_subdirectory( ${exiv2cmake_SOURCE_DIR} ${exiv2cmake_BINARY_DIR})
find_package(exiv2 REQUIRED CONFIG NAMES exiv2)

add_executable(skypasta main.cpp FlirCamera.cpp FlirCamera.h ImageRetriever.cpp ImageRetriever.h Telemetry.cpp Telemetry.h Telemetry.cpp Telemetry.h ImageTag.cpp ImageTag.h lib/json.hpp LinkedList.h)

include_directories(/usr/local/include/spinnaker/ /usr/local/include/exiv2/)
target_link_libraries(skypasta PUBLIC /usr/local/lib/libSpinnaker.dylib exiv2lib)