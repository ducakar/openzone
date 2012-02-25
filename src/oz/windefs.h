/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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
