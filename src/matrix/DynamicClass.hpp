/*
 *  DynamicClass.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/ObjectClass.hpp"

namespace oz
{
namespace matrix
{

class DynamicClass : public ObjectClass
{
  public:

    float mass;
    float lift;

    static ObjectClass* init( const Config* config );

    virtual Object* create( int index, const Point3& pos, Heading heading ) const;
    virtual Object* create( int index, InputStream* istream ) const;

};

}
}
