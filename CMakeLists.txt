cmake_minimum_required(VERSION 2.8)

project (EllipseUtils)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})


SET( EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}" )
IF( NOT EIGEN3_INCLUDE_DIR )
    MESSAGE( FATAL_ERROR "Please point the environment variable EIGEN3_INCLUDE_DIR to the include directory of your Eigen3 installation.")
ENDIF()

set(CMAKE_MODULE_PATH ${EIGEN3_INCLUDE_DIR}/cmake)

find_package(Eigen3 REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(./EllipseUtils)


#add_executable(Test_EllipseUtils Test_EllipseUtils/stdafx.cpp Test_EllipseUtils/stdafx.h Test_EllipseUtils/Test_EllipseUtils.cpp)

#add_executable(Test_EllipseUtils Test_EllipseUtils/stdafx.cpp Test_EllipseUtils/stdafx.h Test_EllipseUtils/Test_EllipseUtils.cpp)
add_subdirectory(EllipseFitToSvg)