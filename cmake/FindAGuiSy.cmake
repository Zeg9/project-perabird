# - Locate AGuiSy library
# This module defines
#  AGUISY_LIBRARY
#  AGUISY_FOUND
#  AGUISY_INCLUDE_DIR

#=============================================================================
# GLStuff.h
# This file is part of AGuiSy
#
# Copyright (C) 2013 - Zeg9
#
# AGuiSy is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# AGuiSy is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with AGuiSy. If not, see <http://www.gnu.org/licenses/>.
#=============================================================================

find_library(AGUISY_LIBRARY
	NAMES aguisy
	PATH_SUFFIXES lib
)

find_package(PackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(AGuiSy
                                  REQUIRED_VARS AGUISY_LIBRARY
                                  )#VERSION_VAR SDL_VERSION_STRING)
