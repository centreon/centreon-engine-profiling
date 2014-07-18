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

# Packaging.
option(WITH_PACKAGE_SH "Build shell-installable package." OFF)
option(WITH_PACKAGE_TGZ "Build gziped tarball package." OFF)
option(WITH_PACKAGE_TBZ2 "Build bzip2'd tarball package." OFF)
option(WITH_PACKAGE_DEB "Build DEB package." OFF)
option(WITH_PACKAGE_RPM "Build RPM package." OFF)
option(WITH_PACKAGE_NSIS "Build NSIS package." OFF)
if (WITH_PACKAGE_SH
    OR WITH_PACKAGE_TGZ
    OR WITH_PACKAGE_TBZ2
    OR WITH_PACKAGE_DEB
    OR WITH_PACKAGE_RPM
    OR WITH_PACKAGE_NSIS)
  # Default settings.
  set(PACKAGE_RELEASE 1)
  set(CPACK_PACKAGE_NAME "centreon-profiling")
  set(CPACK_PACKAGE_VENDOR "Merethis")
  set(CPACK_PACKAGE_VERSION_MAJOR "${PROFILING_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${PROFILING_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${PROFILING_PATCH}")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Centreon Profiling.")
  set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README")
  set(CPACK_PACKAGE_FILE_NAME "centreon-profiling-${PROFILING_VERSION}-${PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "centreon-profiling")
  set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/license.txt")
  set(CPACK_PACKAGE_CONTACT "Dorian Guillois <dguillois@merethis.com>")

  # Generators.
  unset(PACKAGE_LIST)
  if (WITH_PACKAGE_SH)
    list(APPEND CPACK_GENERATOR "STGZ")
    list(APPEND PACKAGE_LIST "Shell-installable package (.sh)")
  endif ()
  if (WITH_PACKAGE_TGZ)
    list(APPEND CPACK_GENERATOR "TGZ")
    list(APPEND PACKAGE_LIST "gziped tarball (.tar.gz)")
  endif ()
  if (WITH_PACKAGE_TBZ2)
    list(APPEND CPACK_GENERATOR "TBZ2")
    list(APPEND PACKAGE_LIST "bzip2'd tarball (.tar.bz2)")
  endif ()
  if (WITH_PACKAGE_DEB)
    list(APPEND CPACK_GENERATOR "DEB")
    list(APPEND PACKAGE_LIST "DEB package (.deb)")
  endif ()
  if (WITH_PACKAGE_RPM)
    list(APPEND CPACK_GENERATOR "RPM")
    list(APPEND PACKAGE_LIST "RPM package (.rpm)")
    set(CPACK_RPM_PACKAGE_RELEASE ${PACKAGE_RELEASE})
    set(CPACK_RPM_PACKAGE_LICENSE "GPLv2")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
    set(CPACK_RPM_CHANGELOG_FILE "${PROJECT_SOURCE_DIR}/ChangeLog")
  endif ()
  if (WITH_PACKAGE_NSIS)
    list(APPEND CPACK_GENERATOR "NSIS")
    list(APPEND PACKAGE_LIST "NSIS package (.exe)")
  endif ()
  string(REPLACE ";" ", " PACKAGE_LIST "${PACKAGE_LIST}")

  # CPack module.
  include(CPack)
else ()
  set(PACKAGE_LIST "None")
endif ()
