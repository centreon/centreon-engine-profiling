##
## Copyright 2013 Merethis
##
## This file is part of Centreon Profiling.
##
## Centreon Profiling is free software: you can redistribute it and/or
## modify it under the terms of the GNU General Public License version 2
## as published by the Free Software Foundation.
##
## Centreon Profiling is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Centreon Profiling. If not, see
## <http://www.gnu.org/licenses/>.
##

# Global options.
cmake_minimum_required(VERSION 2.8)
project("Centreon Profiling" C CXX)

# Set directories.
set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/..")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/inc")
set(INC_DIR "${INCLUDE_DIR}/com/centreon/profiling")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

# Version.
set(PROFILING_MAJOR 1)
set(PROFILING_MINOR 0)
set(PROFILING_PATCH 0)
set(PROFILING_VERSION "${PROFILING_MAJOR}.${PROFILING_MINOR}.${PROFILING_PATCH}")

# Include module to check existing libraries.
include(CheckLibraryExists)

# Set general path.
set(PREFIX "${CMAKE_INSTALL_PREFIX}")
if (WITH_PREFIX)
  set(PREFIX "${WITH_PREFIX}")
  set(CMAKE_INSTALL_PREFIX "${PREFIX}")
endif ()

# Set bin path.
set(PREFIX_BIN "${CMAKE_INSTALL_PREFIX}/bin")
if (WITH_PREFIX_BIN)
  set(PREFIX_BIN "${WITH_PREFIX_BIN}")
endif ()

# Find Centreon Clib's headers.
if (WITH_CENTREON_CLIB_INCLUDE_DIR)
  find_file(
    CLIB_HEADER_FOUND
    "com/centreon/clib/version.hh"
    PATHS "${WITH_CENTREON_CLIB_INCLUDE_DIR}"
    NO_DEFAULT_PATH)
  if (NOT CLIB_HEADER_FOUND)
    message(FATAL_ERROR "Could not find Centreon Clib's headers in ${WITH_CENTREON_CLIB_INCLUDE_DIR}.")
  endif ()
  include_directories("${WITH_CENTREON_CLIB_INCLUDE_DIR}")
  set(CLIB_INCLUDE_DIR "${WITH_CENTREON_CLIB_INCLUDE_DIR}")
elseif (CLIB_FOUND) # Was Centreon Clib detected with pkg-config ?
  if (CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLIB_CFLAGS}")
  else ()
    set(CMAKE_CXX_FLAGS "${CLIB_CFLAGS}")
  endif ()
else ()
  find_path(CLIB_INCLUDE_DIR "com/centreon/clib/version.hh" PATH_SUFFIXES "centreon-clib")
  if (NOT CLIB_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find Centreon Clib's headers (try WITH_CENTREON_CLIB_INCLUDE_DIR).")
  endif ()
  include_directories("${CLIB_INCLUDE_DIR}")
endif ()

# Find Centreon Engine's headers.
if (WITH_CENTREON_ENGINE_INCLUDE_DIR)
  find_file(
    ENGINE_HEADER_FOUND
    "com/centreon/engine/version.hh"
    PATHS "${WITH_CENTREON_ENGINE_INCLUDE_DIR}"
    NO_DEFAULT_PATH)
  if (NOT ENGINE_HEADER_FOUND)
    message(FATAL_ERROR "Could not find Centreon Engine's headers in ${WITH_CENTREON_ENGINE_INCLUDE_DIR}.")
  endif ()
  include_directories("${WITH_CENTREON_ENGINE_INCLUDE_DIR}")
  set(ENGINE_INCLUDE_DIR "${WITH_CENTREON_ENGINE_INCLUDE_DIR}")
elseif (ENGINE_FOUND) # Was Centreon Engine detected with pkg-config ?
  if (CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENGINE_CFLAGS}")
  else ()
    set(CMAKE_CXX_FLAGS "${ENGINE_CFLAGS}")
  endif ()
else ()
  find_path(ENGINE_INCLUDE_DIR "com/centreon/engine/version.hh" PATH_SUFFIXES "centreon-engine")
  if (NOT ENGINE_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find Centreon Engine's headers (try WITH_CENTREON_ENGINE_INCLUDE_DIR).")
  endif ()
  include_directories("${ENGINE_INCLUDE_DIR}")
endif ()

# Generate version header.
message(STATUS "Generating version header file.")
configure_file(
  "${INC_DIR}/version.hh.in"
  "${INC_DIR}/version.hh"
  "@ONLY"
)

# Set sources.
set(
  SOURCES
  "${SRC_DIR}/aggregation.cc"
  "${SRC_DIR}/callback.cc"
  "${SRC_DIR}/entry.cc"
  "${SRC_DIR}/filter.cc"
  "${SRC_DIR}/object_info.cc"
  "${SRC_DIR}/sort.cc"
  "${SRC_DIR}/statistics.cc"
  "${SRC_DIR}/utils.cc"
)

# Set headers.
set(
  HEADERS
  "${INC_DIR}/aggregation.hh"
  "${INC_DIR}/callback.hh"
  "${INC_DIR}/filter.hh"
  "${INC_DIR}/object_info.hh"
  "${INC_DIR}/sort.hh"
  "${INC_DIR}/statistics.hh"
  "${INC_DIR}/utils.hh"
  "${INC_DIR}/version.hh"
)

# Include directories.
include_directories("${INCLUDE_DIR}")

# Create library.
add_library(
  "profilingmod"
  SHARED
  ${SOURCES}
  ${HEADERS}
)
# Prettier name.
set_target_properties("profilingmod" PROPERTIES PREFIX "")


# Install library.
install(
  TARGETS "profilingmod"
  DESTINATION "${PREFIX_BIN}"
  COMPONENT "runtime"
)

# Include build package.
include("package.cmake")

# Print summary.
message(STATUS "")
message(STATUS "Configuration Summary")
message(STATUS "---------------------")
message(STATUS "")
message(STATUS "  Project")
message(STATUS "    - Name                      ${PROJECT_NAME}")
message(STATUS "    - Version                   ${PROFILING_VERSION}")
message(STATUS "")
message(STATUS "  System")
message(STATUS "    - Name                      ${CMAKE_SYSTEM_NAME}")
message(STATUS "    - Version                   ${CMAKE_SYSTEM_VERSION}")
message(STATUS "    - Processor                 ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "")
message(STATUS "  Build")
message(STATUS "    - Compiler                  ${CMAKE_CXX_COMPILER} (${CMAKE_CXX_COMPILER_ID})")
message(STATUS "    - Extra compilation flags   ${CMAKE_CXX_FLAGS}")
message(STATUS "")
message(STATUS "  Installation")
message(STATUS "    - Prefix                    ${PREFIX}")
message(STATUS "    - Prefix bin                ${PREFIX_BIN}")
message(STATUS "    - Package                   ${PACKAGE_LIST}")
message(STATUS "")
