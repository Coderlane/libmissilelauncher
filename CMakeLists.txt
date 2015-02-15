# Root CMakeLists.txt for libmissilelauncher

set(PROJECT_NAME "libmissilelauncher")
cmake_minimum_required(VERSION 2.8.11)
project(${PROJECT_NAME})

include(FindPkgConfig)
include(CMakePackageConfigHelpers)

pkg_search_module(LIBUSB_1 REQUIRED libusb-1.0)

set(LIBMISSILELAUNCHER_LIBRARY "missilelauncher")
set(LIBMISSILELAUNCHER_INCLUDEDIR "${PROJECT_SOURCE_DIR}/include")
set(LIBMISSILELAUNCHER_SRC "${PROJECT_SOURCE_DIR}/src")
set(LIBMISSILELAUNCHER_TEST "${PROJECT_SOURCE_DIR}/test")
set(LIBMISSILELAUNCHER_VERSION_MAJOR "0")
set(LIBMISSILELAUNCHER_VERSION_MINOR "4")
set(LIBMISSILELAUNCHER_VERSION_PATCH "1")
set(LIBMISSILELAUNCHER_VERSION 
	"${LIBMISSILELAUNCHER_VERSION_MAJOR}.${LIBMISSILELAUNCHER_VERSION_MINOR}.${LIBMISSILELAUNCHER_VERSION_PATCH}")


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

add_subdirectory(${LIBMISSILELAUNCHER_INCLUDEDIR})
set(LIBMISSILELAUNCHER_HEADER_FILES ${HEADER_FILES})

add_subdirectory(${LIBMISSILELAUNCHER_SRC})
set(LIBMISSILELAUNCHER_SOURCE_FILES ${SOURCE_FILES})

include_directories(${LIBMISSILELAUNCHER_INCLUDEDIR})
include_directories(${LIBUSB_1_INCLUDE_DIRS})

# lib 
add_library(missilelauncher SHARED ${LIBMISSILELAUNCHER_SOURCE_FILES})

# Link build deps.

target_link_libraries(missilelauncher ${LIBUSB_1_LIBRARIES})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIBRARY_DIR lib CACHE PATH 
	"Installation directory for libraries")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(INSTALL_UDEV_DIR "/etc/udev/rules.d/" CACHE PATH
  "Installation directory for header files")
endif()


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

# Config File Settings

set(LIBMISSILELAUNCHER_INSTALL_PREFIX      "${CMAKE_INSTALL_PREFIX}")
set(LIBMISSILELAUNCHER_INSTALL_EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(LIBMISSILELAUNCHER_INSTALL_LIBDIR 
	"${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}")
set(LIBMISSILELAUNCHER_INSTALL_INCLUDEDIR 
	"${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}")

# Write out cmake config file.
write_basic_package_version_file(libmissilelauncher-config-version.cmake 
	VERSION ${LIBMISSILELAUNCHER_VERSION} COMPATIBILITY SameMajorVersion)

# Configure cmake config file
configure_package_config_file(libmissilelauncher-config.cmake.in 
	${PROJECT_BINARY_DIR}/libmissilelauncher-config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/libmissilelauncher/cmake
	PATH_VARS INSTALL_INCLUDE_DIR INSTALL_LIBRARY_DIR)

# Configure pkgconfig
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libmissilelauncher.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libmissilelauncher.pc @ONLY)

# Set install targets
install(TARGETS ${LIBMISSILELAUNCHER_LIBRARY} 
	DESTINATION ${LIBMISSILELAUNCHER_INSTALL_LIBDIR} COMPONENT shlib)
install(FILES ${LIBMISSILELAUNCHER_HEADER_FILES}
	DESTINATION "${LIBMISSILELAUNCHER_INSTALL_INCLUDEDIR}" COMPONENT dev)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libmissilelauncher.pc
	DESTINATION "${INSTALL_PKGCONFIG_DIR}" COMPONENT dev)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	install(FILES "${PROJECT_SOURCE_DIR}/99-missile-launcher.rules"
		DESTINATION "${INSTALL_UDEV_DIR}" COMPONENT dev)
endif()

install(FILES
  "${PROJECT_BINARY_DIR}/libmissilelauncher-config.cmake"
  "${PROJECT_BINARY_DIR}/libmissilelauncher-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

### CPACK
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_CONTACT "" CACHE STRING 
	"The contact information for the package distributor")

set(CPACK_PACKAGE_NAME "libmissilelauncher")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A library to enable control of some foam missle launchers.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${LIBMISSILELAUNCHER_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${LIBMISSILELAUNCHER_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${LIBMISSILELAUNCHER_VERSION_PATCH}")
set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_TOPLEVEL_TAG "${CMAKE_SYSTEM_NAME}-${CPACK_SYSTEM_PROCESSOR}")
include(CPack)
