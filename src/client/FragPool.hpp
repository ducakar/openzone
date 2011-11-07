/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/FragPool.hpp
 */

#pragma once

#include "matrix/Frag.hpp"

#include "client/common.hpp"

namespace oz
{
namespace client
{

class SMM;

class FragPool
{
  public:

    static const float FRAG_RADIUS;

  private:

    static const float SQRT_3_THIRDS;
    static const float DIM;
    static const int   MAX_FRAGS = 64;
    static const int   LINE_LENGTH = 64;

    struct FragInfo
    {
      SMM* model;
      int  id;
    };

    static char buffer[LINE_LENGTH];

    uint vao;
    uint vbo;

    Vector<FragInfo> frags;

    int firstStone;
    int nStones;
    int firstWood;
    int nWoods;
    int firstMetal;
    int nMetals;
    int firstMeat;
    int nMeats;

    int debrisIds[11];
    SMM* debris[11];

    void loadFrags();

  public:

    FragPool();

    void bindVertexArray() const;

    void draw( const Frag* frag );

    void load();
    void unload();

};

extern FragPool fragPool;

}
}
