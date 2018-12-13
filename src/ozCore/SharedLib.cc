/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include "SharedLib.hh"

#if defined(__native_client__)
#elif defined(_WIN32)
# include <windows.h>
#else
# include <dlfcn.h>
#endif

namespace oz
{

#ifdef __native_client__
const bool SharedLib::IS_SUPPORTED = false;
#else
const bool SharedLib::IS_SUPPORTED = true;
#endif

SharedLib::SharedLib(const char* name)
{
#if defined(__native_client__)
  static_cast<void>(name);
#elif defined(_WIN32)
  handle_ = static_cast<void*>(LoadLibrary(name));
#else
  handle_ = dlopen(name, RTLD_NOW);
#endif
}

SharedLib::~SharedLib()
{
  close();
}

SharedLib::SharedLib(SharedLib&& other) noexcept
  : SharedLib()
{
  swap(*this, other);
}

SharedLib& SharedLib::operator=(SharedLib&& other) noexcept
{
  swap(*this, other);
  return *this;
}

SharedLib::Function* SharedLib::get(const char* symbol) const
{
  if (handle_ == nullptr) {
    return nullptr;
  }

#if defined(__native_client__)
  static_cast<void>(symbol);
  return nullptr;
#elif defined(_WIN32)
  return reinterpret_cast<Function*>(GetProcAddress(static_cast<HMODULE>(handle_), symbol));
#else
  return reinterpret_cast<Function*>(dlsym(handle_, symbol));
#endif
}

void SharedLib::close()
{
  if (handle_ != nullptr) {
#if defined(__native_client__)
#elif defined(_WIN32)
    FreeLibrary(static_cast<HMODULE>(handle_));
#else
    dlclose(handle_);
#endif
    handle_ = nullptr;
  }
}

}
