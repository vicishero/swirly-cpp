# The Restful Matching-Engine.
# Copyright (C) 2013, 2018 Swirly Cloud Limited.
#
# This program is free software; you can redistribute it and/or modify it under the terms of the
# GNU General Public License as published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this program; if
# not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.

set(fbs_SOURCES
  Asset.fbs
  Exec.fbs
  Instr.fbs
  Market.fbs
  Order.fbs
  Posn.fbs
  Types.fbs)

build_flatbuffers(
  "${fbs_SOURCES}"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  swirly-fbs
  ""
  "${CMAKE_BINARY_DIR}/include/swirly/fbs"
  ""
  "")

install(FILES ${fbs_SOURCES} DESTINATION etc/fbs COMPONENT config)
install(FILES
  "${CMAKE_BINARY_DIR}/include/swirly/fbs/Asset_generated.h"
  "${CMAKE_BINARY_DIR}/include/swirly/fbs/Exec_generated.h"
  "${CMAKE_BINARY_DIR}/include/swirly/fbs/Instr_generated.h"
  "${CMAKE_BINARY_DIR}/include/swirly/fbs/Market_generated.h"
  "${CMAKE_BINARY_DIR}/include/swirly/fbs/Order_generated.h"
  "${CMAKE_BINARY_DIR}/include/swirly/fbs/Posn_generated.h"
  "${CMAKE_BINARY_DIR}/include/swirly/fbs/Types_generated.h"
  DESTINATION include/swirly/fbs
  COMPONENT header
)
