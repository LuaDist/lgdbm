# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

PROJECT(lgdbm C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE(dist.cmake)

# Find GDBM
FIND_PACKAGE(GDBM REQUIRED)
INCLUDE_DIRECTORIES(${GDBM_INCLUDE_DIRS})

# Build
ADD_LUA_MODULE(gdbm lgdbm.c)
TARGET_LINK_LIBRARIES(gdbm ${GDBM_LIBRARIES})

# Install all files and documentation
INSTALL(TARGETS gdbm  DESTINATION ${INSTALL_CMOD})
INSTALL(FILES README DESTINATION ${INSTALL_DATA})
INSTALL(FILES test.lua DESTINATION ${INSTALL_TEST})
