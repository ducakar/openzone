/*
 *  Material.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  struct Material
  {
    static const int STRUCT_BIT  = 0x00000001;
    static const int SLICK_BIT   = 0x00000002;
    static const int WATER_BIT   = 0x00000004;
    static const int LADDER_BIT  = 0x00000008;
    static const int TERRAIN_BIT = 0x00000010;
    static const int WOOD_BIT    = 0x00000020;
    static const int VOID_BIT    = 0x00000040;
  };

}
