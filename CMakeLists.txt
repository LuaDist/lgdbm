# Copyright (C) 2007-2012 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( lgdbm C )
cmake_minimum_required ( VERSION 2.8 )
include ( cmake/dist.cmake )
include ( lua )

find_package ( GDBM REQUIRED )
include_directories ( ${GDBM_INCLUDE_DIRS} )

install_lua_module ( gdbm lgdbm.c LINK ${GDBM_LIBRARIES} )
install_data ( README )
install_test ( test.lua )
