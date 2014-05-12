#
#
#
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(missilelauncher)

#Uncomment to make makefiles verbose
#set(CMAKE_VERBOSE_MAKEFILE ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu99 -Wall -Wextra -I/usr/include/libusb-1.0")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG -O0")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O3")

#Source Code
ADD_SUBDIRECTORY(src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Exe
ADD_LIBRARY(libmissilelauncher ${SOURCE_FILES})

# Copy over resoureces to build dir

# Find Build Deps
#INCLUDE(FindPkgConfig)
