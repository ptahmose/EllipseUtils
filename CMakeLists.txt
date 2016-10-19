cmake_minimum_required(VERSION 2.8)

project (EllipseUtils)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

#if ($ENV{EIGEN3_INCLUDE_DIR})
#	message("Environment variable was set")
#endif

if(!EIGEN3_INCLUDE_DIR)
  message("Please specify the path where Eigen3 is found -e. g. 'cmake -D EIGEN3_INCLUDE_DIR=/usr/include/eigen3'")
endif()

#SET( EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}" )
list(APPEND CMAKE_MODULE_PATH /usr/include/eigen3)
#IF( NOT EIGEN3_INCLUDE_DIR )
#    MESSAGE( FATAL_ERROR "Please point the environment variable EIGEN3_INCLUDE_DIR to the include directory of your Eigen3 installation.")
#ENDIF()

#set(CMAKE_MODULE_PATH ${EIGEN3_INCLUDE_DIR}/cmake)
set(CMAKE_MODULE_PATH ./cmake)

find_package(Eigen3 3.1.2)

list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES ${EIGEN3_INCLUDE_DIR})
#list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "./EllipseUtils")

#include_directories("/usr/include/eigen3")
#include_directories(./EllipseUtils)
include_directories(${ADDITIONAL_INCLUDE_DIRECTORIES})

message(${ADDITIONAL_INCLUDE_DIRECTORIES})

set (CMAKE_CXX_FLAGS "--std=c++11")


#add_executable(Test_EllipseUtils Test_EllipseUtils/stdafx.cpp Test_EllipseUtils/stdafx.h Test_EllipseUtils/Test_EllipseUtils.cpp)

#add_executable(Test_EllipseUtils Test_EllipseUtils/stdafx.cpp Test_EllipseUtils/stdafx.h Test_EllipseUtils/Test_EllipseUtils.cpp)
add_subdirectory(EllipseFitToSvg)
