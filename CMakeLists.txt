cmake_minimum_required(VERSION 3.18)
project(FMCA CXX)
# Fetch dependency 
include(FetchContent)
 # More verbose Output for libraries using set_package_properties

include(FeatureSummary)



message(STATUS "
****************************************************************************
*                  ______   __    __    ______    ______                   * 
*                 |   ___| |  \\  /  |  /   ___|  /      \\                  *
*                 |   __   |   \\/   | |   /     |   /\\   |                 *
*                 |   __|  |        | |  |      |  |__|  |                 *
*                 |  |     |  |\\/|  | |  |      |   __   |                 *
*                 |  |     |  |  |  | |   \\___  |  |  |  |                 *
*                 |__|     |__|  |__|  \\______| |__|  |__|                 *
*                                                                          *
****************************************************************************
This is FMCA, the Fast Multiresolution Covariance Analysis package.

Copyright (c) 2022, Michael Multerer
All rights reserved.

This source code is subject to the GNU Affero General Public License v3.0
license and without any warranty, see <https://github.com/muchip/FMCA>
for further information.
")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.18.0")
  find_package(Python 3.9 COMPONENTS Interpreter Development.Module Development.Embed REQUIRED)
else()
  find_package(Python 3.9 COMPONENTS Interpreter Development REQUIRED)
endif()
find_package(OpenMP REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Pybind11 CONFIG)

set(PYBIND11_CPP_STANDARD -std=c++11)
message("${CMAKE_MODULE_PATH}")
message("${eigen3_INCLUDE_DIR}")
message("${pybind11_INCLUDE_DIR}")
message("${Python3_ROOT_DIR}/include")

set (FMCA_PATH ${PROJECT_SOURCE_DIR})
message("${PROJECT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS "-std=c++11 -I${FMCA_PATH} ")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set( PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

add_subdirectory(tests)
add_subdirectory(py)
enable_testing()
