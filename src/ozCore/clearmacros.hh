/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/clearmacros.hh
 *
 * This header clears ozCore macros that might interfere with other libraries.
 *
 * It undefines `soft_assert()`, `hard_assert()`, `foreach()`, `float4()` and `uint4()` and macros
 * that might be defined for older GCC versions only to ensure for C++11 compatibility. It is not
 * included by `\<oz/oz.hh\>`, one should include it before headers that conflict with liboz.
 */

#pragma once

#undef soft_assert
#undef hard_assert
#undef foreach
#undef vShuffle

// Might be defined in `config.hh` for C++11 compatibility when using older GCC versions.
#undef constexpr
#undef noexcept
#undef nullptr
#undef override
#undef final
