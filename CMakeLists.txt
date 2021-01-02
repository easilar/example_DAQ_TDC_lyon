################################################################################
# CMakeLists.txt for dataQuest project.
# Shchablo Konstantin (Shchablo@gmail.com)
################################################################################

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(b2r LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PLATFORM_INDEPENDENT_CODE ON)


# MODULE -----------------------------------------------------------------------

  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
  
  ## CERN_ROOT
  #list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/cmake/)
  find_package(ROOT CONFIG REQUIRED)
  set(ROOT_DIR $ENV{ROOTSYS})
  #include("${ROOT_DIR}/cmake/modules/RootNewMacros.cmake")  
  include("${ROOT_DIR}/RootMacros.cmake")  
  
  include("${ROOT_USE_FILE}")
  
  ## ZLIB
    find_package(ZLIB REQUIRED)
    include_directories(${ZLIB_IBCLUDE_DIRS})
  ##
#-------------------------------------------------------------------------------

# PROJECT INCLUDES -------------------------------------------------------------
  include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  ## main 
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
  ##
#-------------------------------------------------------------------------------

# PROJECT SRC ------------------------------------------------------------------
file(GLOB MAIN_SRC "*.cpp")
  ## main
  file(GLOB DQ_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
  ##
  set(CPPS ${MAIN_SRC} ${DQ_SRC})


#-------------------------------------------------------------------------------

# COMPILER SETTING -------------------------------------------------------------

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. 
                      Please update your C++ compiler.")
endif()

add_definitions(-Wall -O3)

#Link to ROOT.

add_executable(${PROJECT_NAME} ${CPPS})
target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
# LIBRARIES --------------------------------------------------------------------
file(GLOB PCM_FILES "${PROJECT_BINARY_DIR}/*.pcm")
#-------------------------------------------------------------------------------
################################################################################
