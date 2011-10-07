/*
 *  WeaponClass.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/DynamicClass.hpp"

namespace oz
{

  class WeaponClass : public DynamicClass
  {
    public:

      Map<const ObjectClass*> allowedUsers;

      String onShot;

      int    nRounds;
      float  shotInterval;

      static ObjectClass* init( const Config* config );

      virtual Object* create( int index, const Point3& pos ) const;
      virtual Object* create( int index, InputStream* istream ) const;

      void fillAllowedUsers();
  };

}
