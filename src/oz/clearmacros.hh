/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/clearmacros.hh
 *
 * This header clears liboz macros that might interfere with other libraries.
 *
 * It undefines `soft_assert()`, `hard_assert()` and `foreach()` and macros that might be defined
 * for older GCC versions only to ensure for C++11 compatibility. It is not included by
 * `\<oz/oz.hh\>`, one should include it manually before headers that conflict with liboz.
 */

#pragma once

#undef soft_assert
#undef hard_assert
#undef foreach

// Defined only for older GCC versions for C++11 compatibility.
#undef constexpr
#undef noexcept
#undef override
#undef null
