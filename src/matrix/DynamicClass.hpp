/*
 *  DynamicClass.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Object.hpp"
#include "matrix/ObjectClass.hpp"

namespace oz
{

  class DynamicClass : public ObjectClass
  {
    private:

      static const int BASE_FLAGS = Object::DYNAMIC_BIT;

    public:

      float mass;
      float lift;

      static ObjectClass* init( const Config* config );

      virtual Object* create( int index, const Point3& pos ) const;
      virtual Object* create( int index, InputStream* istream ) const;

  };

}
