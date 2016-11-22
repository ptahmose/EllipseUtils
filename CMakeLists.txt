cmake_minimum_required(VERSION 2.8)

project (EllipseUtils)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE )
endif()

if(NOT EIGEN3_INCLUDE_DIR)
  if (DEFINED ENV{EIGEN3_INCLUDE_DIR})
    set(EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}")
  endif()
  if (NOT EIGEN3_INCLUDE_DIR)
    message(FATAL_ERROR "Please specify the path where Eigen3 is found - either by specifying it on the command-line, or by defining an environment variable (e. g. 'cmake -DEIGEN3_INCLUDE_DIR=/usr/include/eigen3')")
  endif()
endif()

list(APPEND CMAKE_PREFIX_PATH ${EIGEN3_INCLUDE_DIR})
message(${EIGEN3_INCLUDE_DIR})

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Eigen3 3.1.2)

list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES ${EIGEN3_INCLUDE_DIR})

include_directories(${ADDITIONAL_INCLUDE_DIRECTORIES})

#message(${ADDITIONAL_INCLUDE_DIRECTORIES})



macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" || CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)


add_subdirectory(EllipseFitToSvg)

set_target_properties(EllipseFitToSvg 
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/EllipseFitToSvg/")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Test" DESTINATION "${CMAKE_BINARY_DIR}/")
