################################################################################
# CMakeLists.txt for dataQuest project.
# Shchablo Konstantin (Shchablo@gmail.com)
################################################################################

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(example)

# MODULE -----------------------------------------------------------------------
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
  ## ZLIB
    find_package(ZLIB REQUIRED)
    include_directories(${ZLIB_IBCLUDE_DIRS})
  ##
#-------------------------------------------------------------------------------

# PROJECT INCLUDES -------------------------------------------------------------
  include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  ## example 
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
  ##
#-------------------------------------------------------------------------------

# PROJECT SRC ------------------------------------------------------------------
file(GLOB MAIN_SRC "*.cpp")
  ## example 
  file(GLOB EXAMPLE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
  ##
  set(CPPS ${MAIN_SRC} ${EXAMPLE_SRC})
#-------------------------------------------------------------------------------

# COMPILER SETTING -------------------------------------------------------------

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
  CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
  if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  else()
    if(COMPILER_SUPPORTS_CXX11)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
      message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. 
                      Please update your C++ compiler.")
    endif()
  endif()
endif()

add_definitions(-Wall -O3)

add_executable(${PROJECT_NAME} ${CPPS})
# LIBRARIES --------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
#-------------------------------------------------------------------------------
################################################################################
