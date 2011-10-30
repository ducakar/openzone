/*
 *  Terra.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Terra.hpp"

namespace oz
{
namespace build
{

class Terra
{
  private:

    matrix::Terra::Quad quads[matrix::Terra::VERTS][matrix::Terra::VERTS];

    String name;
    String waterTexture;
    String detailTexture;
    String mapTexture;

    void load();
    void saveMatrix();
    void saveClient();

    explicit Terra( const char* name );

  public:

    static void prebuild( const char* name );

};

}
}
