cmake_minimum_required(VERSION 3.15)
project(skypasta)

set(CMAKE_CXX_STANDARD 14)

add_executable(skypasta main.cpp FlirCamera.cpp FlirCamera.h ImageRetriever.cpp ImageRetriever.h)
include_directories(/usr/local/include/spinnaker/)
target_link_libraries(skypasta PUBLIC /usr/local/lib/libSpinnaker.dylib)

find_package(Boost 1.72.0 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
add_executable(BoostTest main.cpp)
target_link_libraries(BoostTest ${Boost_LIBRARIES})