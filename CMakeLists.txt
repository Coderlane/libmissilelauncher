#
#
#
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(MissileLauncher)

include(FindPkgConfig)
include(CMakePackageConfigHelpers)

set(MAJOR_VERSION 0)
set(MINOR_VERSION 2)
set(PATCH_VERSION 0)
set(LIBMISSILELAUNCHER_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})

write_basic_package_version_file(libmissilelauncher-config-version.cmake 
	VERSION ${LIBMISSILELAUNCHER_VERSION} COMPATIBILITY SameMajorVersion)

#Uncomment to make makefiles verbose
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Determine the system type
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLINUX")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDARWIN")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWINDOWS")
else()
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNKNOWN")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -O0")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3")

#Source Code
add_subdirectory(src)
include_directories(${PROJECT_SOURCE_DIR}/src)

# lib 
add_library(missilelauncher SHARED ${SOURCE_FILES} ${HEADER_FILES})

# Find Build Deps
pkg_search_module(LIBUSB_1 REQUIRED libusb-1.0)
include_directories(${LIBUSB_1_INCLUDE_DIRS})
target_link_libraries(missilelauncher ${LIBUSB_1_LIBRARIES})

### Installation

set_target_properties(missilelauncher PROPERTIES
 	PUBLIC_HEADER "src/libmissilelauncher.h")

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIBRARY_DIR lib CACHE PATH 
	"Installation directory for libraries")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")

# CMake install dir
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/libmissilelauncher)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# CMake install dir
if(WIN32 AND NOT CYGWIN)
	set(DEF_INSTALL_PKGCONFIG_DIR pkgconfig)
else()
	set(DEF_INSTALL_PKGCONFIG_DIR lib/pkgconfig)
endif()
set(INSTALL_PKGCONFIG_DIR ${DEF_INSTALL_PKGCONFIG_DIR} CACHE PATH
  "Installation directory for pkg-config files")

configure_package_config_file(libmissilelauncher-config.cmake.in 
	${PROJECT_BINARY_DIR}/libmissilelauncher-config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/libmissilelauncher/cmake
	PATH_VARS INSTALL_INCLUDE_DIR INSTALL_LIBRARY_DIR)

install(TARGETS missilelauncher 
  DESTINATION "${INSTALL_LIBRARY_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/libmissilelauncher"
    COMPONENT dev)

install(FILES
  "${PROJECT_BINARY_DIR}/libmissilelauncher-config.cmake"
  "${PROJECT_BINARY_DIR}/libmissilelauncher-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

install(FILES
	"${PROJECT_SOURCE_DIR}/libmissilelauncher.pc"
	DESTINATION "${INSTALL_PKGCONFIG_DIR}" COMPONENT dev)

### CPACK
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "libmissilelauncher")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A library to enable control of some foam missle launchers.")
set(CPACK_PACKAGE_VENDOR "Travis Lane")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION})
set(CPACK_PACKAGE_VERSION ${LIBMISSILELAUNCHER_VERSION})
set(CPACK_PACKAGE_CONTACT "Travis Lane")
INCLUDE(CPack)
