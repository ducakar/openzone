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

#pragma once

/**
 * @file oz/windefs.h
 *
 * Windows-specific defines.
 *
 * This header should always be included before <tt>\<windows.h\></tt> and possibly also before any
 * standard C/C++ header as it may include Windows headers internally.
 */

#ifdef _WIN32

// At least Windows 2000 required.
# define WINVER 0x0500

// At least Visual Studio 2005 run-time libraries required.
# define __MSVCRT_VERSION__ 0x0800

// At least Internet Explorer 6.0 libraries required.
# define _WIN32_IE 0x0600

// Don't import all crap from Windows headers.
# define WIN32_LEAN_AND_MEAN

// Don't define min and max macros as they interfere with oz::min and oz::max.
#ifndef NOMINMAX
# define NOMINMAX
#endif

#endif
