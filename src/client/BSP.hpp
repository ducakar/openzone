/*
 *  BSP.hpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/Struct.hpp"
#include "matrix/BSP.hpp"

#include "client/Mesh.hpp"

namespace oz
{
namespace client
{

class BSP
{
  private:

    int  id;
    int  flags;
    Mesh mesh;

    const matrix::BSP* bsp;

    void playSound( const Struct::Entity* entity, int sample ) const;
    void playContSound( const Struct::Entity* entity, int sample ) const;

  public:

    bool isLoaded;

    explicit BSP( int id );
    ~BSP();

    void draw( const Struct* str, int mask ) const;
    void play( const Struct* str ) const;

    void load();

};

}
}
