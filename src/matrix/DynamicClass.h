/*
 *  DynamicClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/Object.h"
#include "matrix/ObjectClass.h"

namespace oz
{

  class DynamicClass : public ObjectClass
  {
    private:

      static const int BASE_FLAGS = Object::DYNAMIC_BIT;

    public:

      float mass;
      float lift;

      static ObjectClass* init( const String& name, const Config* config );

      virtual Object* create( int index, const Vec3& pos ) const;
      virtual Object* create( int index, InputStream* istream ) const;

  };

}
