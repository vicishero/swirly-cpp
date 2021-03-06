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
#include "MemAlloc.hpp"

namespace swirly {
inline namespace app {
using namespace std;

SWIRLY_WEAK void* alloc(size_t size);
SWIRLY_WEAK void* alloc(size_t size, align_val_t al);
SWIRLY_WEAK void dealloc(void* ptr, size_t size) noexcept;
SWIRLY_WEAK void dealloc(void* ptr, size_t size, align_val_t al) noexcept;

void* alloc(size_t size)
{
    return ::operator new(size);
}

void* alloc(size_t size, align_val_t al)
{
    return ::operator new(size, al);
}

void dealloc(void* ptr, size_t size) noexcept
{
#if __cpp_sized_deallocation
    ::operator delete(ptr, size);
#else
    ::operator delete(ptr);
#endif
}

void dealloc(void* ptr, size_t size, align_val_t al) noexcept
{
#if __cpp_sized_deallocation
    ::operator delete(ptr, size, al);
#else
    ::operator delete(ptr, al);
#endif
}

} // namespace app
} // namespace swirly
