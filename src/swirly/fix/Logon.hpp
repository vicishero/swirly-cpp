/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#ifndef SWIRLY_FIX_LOGON_HPP
#define SWIRLY_FIX_LOGON_HPP

#include <swirly/fix/Field.hpp>

namespace swirly {
inline namespace fix {

struct Logon {
    EncryptMethod encrypt_method;
    HeartBtInt heart_bt_int;
};

SWIRLY_API std::ostream& operator<<(std::ostream& os, const Logon& body);

SWIRLY_API void parse_body(std::string_view msg, std::size_t body_off, Logon& body);

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_LOGON_HPP