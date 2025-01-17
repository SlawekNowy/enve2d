# enve2d - https://github.com/enve2d
#
# Copyright (c) enve2d developers
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.9)
project(enve2dproject VERSION 0.9.0)

# package for Ubuntu 22.04+ or similar
if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
    set(CPACK_PACKAGE_NAME "enve2d")
    set(CPACK_PACKAGE_DESCRIPTION "2D Animation")
    set(CPACK_PACKAGE_VENDOR "enve2d developers")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt5-image-formats-plugins, libqt5multimedia5-plugins")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "enve2d developers")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/enve2d/enve2d")
    set(CPACK_DEBIAN_FILE_NAME "enve2d.deb")
    include(CPack)
endif()

add_subdirectory(src/core)
add_subdirectory(src/app)
