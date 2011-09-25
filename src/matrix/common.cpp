/*
 *  common.hpp
 *
 *  Common matrix includes and definitions
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/common.hpp"

namespace oz
{
  // ensure epsilon is big enough for a 4 km x 4 km world (1 mm should do)
  // EPSILON = Orbis::DIM * 4.0f * Math::EPSILON
#ifdef OZ_MINGW
  // FIXME WTF __FLT_EPSILON__ works, but not Math::EPSILON? Compiler/linker bug?
  const float EPSILON = 2048.0f * 4.0f * __FLT_EPSILON__;
#else
  const float EPSILON = 2048.0f * 4.0f * Math::EPSILON;
#endif

}
