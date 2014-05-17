#
#
#
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(MissileLauncher)

INCLUDE(FindPkgConfig)
INCLUDE(CMakePackageConfigHelpers)

set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)
set(PATCH_VERSION 0)
set(LIBMISSILELAUNCHER_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})

WRITE_BASIC_PACKAGE_VERSION_FILE(libmissilelauncher-config-version.cmake 
	VERSION ${LIBMISSILELAUNCHER_VERSION} COMPATIBILITY SameMajorVersion)

#Uncomment to make makefiles verbose
#set(CMAKE_VERBOSE_MAKEFILE ON)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wextra")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -DDEBUG -O0")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG -O3")

#Source Code
ADD_SUBDIRECTORY(src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# lib 
ADD_LIBRARY(missilelauncher ${SOURCE_FILES})

# Find Build Deps
PKG_SEARCH_MODULE(LIBUSB_1 REQUIRED libusb-1.0)
INCLUDE_DIRECTORIES(${LIBUSB_1_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(missilelauncher ${LIBUSB_1_LIBRARIES})

### Installation

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIBRARY_DIR lib CACHE PATH 
	"Installation directory for libraries")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")

if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/libmissilelauncher)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")
 
# Make relative paths absolute
foreach(p LIB INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)
 
set_target_properties(missilelauncher PROPERTIES
 	PUBLIC_HEADER "src/libmissilelauncher.h")
 
install(TARGETS missilelauncher 
	EXPORT MissileLauncherTargets
  DESTINATION "${INSTALL_LIBRARY_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/libmissilelauncher"
    COMPONENT dev)

	#INSTALL(TARGETS missilelauncher DESTINATION lib)
	#INSTALL(FILES ${HEADER_FILES} DESTINATION include/libmissilelauncher)

# Add all targets to the build-tree export set
export(TARGETS missilelauncher
	FILE "${PROJECT_BINARY_DIR}/libmissilelauncher-targets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
EXPORT(PACKAGE MissileLauncher)
 
# Create the libmissilelauncher-config.cmake and Libmissilelauncher-config files
#EXPORT(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
#set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
#configure_file(libmissilelauncher.cmake.in
#  "${PROJECT_BINARY_DIR}/libmissilelauncher.cmake" @ONLY)

# ... for the install tree
#set(CONF_INCLUDE_DIRS "\${LIBMISSILELAUNCHER_CMAKE_DIR}/${REL_INCLUDE_DIR}")
#configure_file(libmissilelauncher-config.cmake.in
#  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libmissilelauncher-config.cmake" @ONLY)
# ... for both

configure_package_config_file(libmissilelauncher-config.cmake.in 
	${PROJECT_BINARY_DIR}/libmissilelauncher-config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/libmissilelauncher/cmake
	PATH_VARS INSTALL_INCLUDE_DIR INSTALL_LIBRARY_DIR)

install(FILES
  "${PROJECT_BINARY_DIR}/libmissilelauncher-config.cmake"
  "${PROJECT_BINARY_DIR}/libmissilelauncher-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
 
# Install the export set for use with the install-tree
#install(EXPORT MissileLauncherTargets DESTINATION
# "${INSTALL_CMAKE_DIR}" COMPONENT dev)
