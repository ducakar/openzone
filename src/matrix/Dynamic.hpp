/*
 *  Dynamic.hpp
 *
 *  Dynamic object
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/Object.hpp"

namespace oz
{
namespace matrix
{

// dynamic object abstract class
class Dynamic : public Object
{
  public:

    static Pool<Dynamic, 4096> pool;

    Vec3  velocity;
    Vec3  momentum;   // desired velocity
    Vec3  floor;      // if on ground, used as floor normal, it is not set if on another object

    int   parent;     // index of container object (if object isn't positioned in the world,
                      // it has to be contained in another object, otherwise it will be removed)
    int   lower;      // index of the lower object
    float depth;      // how deep under water the object's lower bound is

    float mass;
    float lift;

    Dynamic() : velocity( Vec3::ZERO ), momentum( Vec3::ZERO ), parent( -1 ), lower( -1 ),
        depth( 0.0f )
    {}

    virtual void readFull( InputStream* istream );
    virtual void writeFull( OutputStream* ostream ) const;
    virtual void readUpdate( InputStream* istream );
    virtual void writeUpdate( OutputStream* ostream ) const;

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
