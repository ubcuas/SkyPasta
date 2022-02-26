cmake_minimum_required( VERSION 3.13 )
set( CMAKE_CXX_STANDARD 14 )
project( skypasta )

# Libuuas options
option( UUAS_ENABLE_CLANG_TIDY "" ON )

# Set compile flags
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Find clang-tidy executable
find_program( CLANG_TIDY_EXE NAMES "clang-tidy" )
if( NOT CLANG_TIDY_EXE )
  message( STATUS "clang-tidy not found." )
else()
  message( STATUS "clang-tidy found: ${CLANG_TIDY_EXE}" )
  set( DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks=*,-fuchsia-default-arguments,-fuchsia-default-arguments-calls,-modernize-use-trailing-return-type,-*-vararg" )
endif()

# Libexiv2 dependency
# Version 0.27.1 has an ARM compilation issue. The issue is fixed on a branch but a new version hasn't been released...
include(FetchContent)
FetchContent_Declare( exiv2cmake URL https://www.exiv2.org/releases/exiv2-0.27.0-Source.tar.gz ) 
FetchContent_GetProperties( exiv2cmake )
if ( NOT exiv2cmake_POPULATED )
  FetchContent_Populate( exiv2cmake )
endif()
option( EXIV2_BUILD_SAMPLES "" OFF )
option( EXIV2_BUILD_EXIV2_COMMAND "" OFF )
add_subdirectory( ${exiv2cmake_SOURCE_DIR} ${exiv2cmake_BINARY_DIR} )

# Spinnaker dependency
find_library( SPINNAKER_LIB "Spinnaker" HINTS "/opt/spinnaker/lib" "/usr/lib" )
if( NOT SPINNAKER_LIB )
  message( FATAL_ERROR "You need to install the spinnaker library first: https://www.flir.ca/products/spinnaker-sdk/" )
endif()
set( SPINNAKER_LIB_INCLUDES "/opt/spinnaker/include" "/usr/include/spinnaker" )

add_executable( skypasta
  ${CMAKE_SOURCE_DIR}/src/main.cpp
  ${CMAKE_SOURCE_DIR}/src/FlirCamera.cpp
  ${CMAKE_SOURCE_DIR}/src/GenericUSBCamera.cpp
  ${CMAKE_SOURCE_DIR}/src/ImageRetriever.cpp
  ${CMAKE_SOURCE_DIR}/src/Telemetry.cpp
  ${CMAKE_SOURCE_DIR}/src/Telemetry.cpp
  ${CMAKE_SOURCE_DIR}/src/ImageTag.cpp
)

target_include_directories( skypasta SYSTEM PUBLIC ${SPINNAKER_LIB_INCLUDES} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/include )
target_link_libraries( skypasta PUBLIC ${SPINNAKER_LIB} exiv2lib pthread opencv_core opencv_imgproc opencv_highgui opencv_ml opencv_video opencv_videoio opencv_imgcodecs )
target_compile_options( skypasta PUBLIC "-Wall" "-Wextra" )
