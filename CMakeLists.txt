cmake_minimum_required(VERSION 3.4...3.18)
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


# try to hack windows compiler 
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEFAULT "/DWIN32 /D_WINDOWS /W3 /GR /EHsc")
    if("${CMAKE_CXX_FLAGS}" STREQUAL "${CMAKE_CXX_FLAGS_DEFAULT}")
        string(APPEND CMAKE_CXX_FLAGS " /fp:precise /bigobj /wd4996 /source-charset:utf-8 ")
        set(MSVC_MP ${CPUCores})

        if(CMAKE_VERSION VERSION_LESS 3.11.0)
            if(MSVC_MP EQUAL 0)
                string(APPEND CMAKE_CXX_FLAGS " /MP")
            elseif(MSVC_MP GREATER 1)
                string(APPEND CMAKE_CXX_FLAGS " /MP${MSVC_MP}")
            endif()
        else()
            if(MSVC_MP EQUAL 0)
                add_compile_options($<$<OR:$<COMPILE_LANGUAGE:C>,$<COMPILE_LANGUAGE:CXX>>:/MP>)
            elseif(MSVC_MP GREATER 1)
                add_compile_options($<$<OR:$<COMPILE_LANGUAGE:C>,$<COMPILE_LANGUAGE:CXX>>:/MP${MSVC_MP}>)
            endif()
        endif()
    endif()
endif()


find_package(OpenMP COMPONENTS  CXX)

if(OpenMP_FOUND)
    string(APPEND CMAKE_CXX_FLAGS " ${OpenMP_CXX_FLAGS}")
    string(APPEND CMAKE_EXE_LINKER_FLAGS "${OpenMP_EXE_LINKER_FLAGS}")
endif()


find_package(Eigen3 REQUIRED)
find_package(Pybind11 CONFIG)


set(cxx_flags 
	$<$<CXX_COMPILER_ID:MSVC>:-D_SCL_SECURE_NO_WARNINGS /openmp /std:c++11>           #Add MSVC-specific compiler flags here
	$<$<CXX_COMPILER_ID:GNU>:-Wall -m64 -fopenmp -O3 -std=c++11 -Wno-unused-function> #Add GCC/Clang-specific compiler flags here
	)


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
