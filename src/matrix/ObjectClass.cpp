/*
 *  ObjectClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#include "precompiled.hpp"

#include "ObjectClass.hpp"

namespace oz
{

  ObjectClass::ObjectClass( const char *file )
  {
    logFile.println( "Loading class %s {", file );
    logFile.indent();

    Config config;
    config.load( file );

    OZ_CLASS_READ_FLOAT( dim.x, 1.0f );
    OZ_CLASS_READ_FLOAT( dim.y, 1.0f );
    OZ_CLASS_READ_FLOAT( dim.z, 1.0f );

    OZ_CLASS_READ_INT( flags, 0 );
    OZ_CLASS_READ_INT( type, 0 );
    OZ_CLASS_READ_FLOAT( damage, 1.0f );

    OZ_CLASS_READ_STRING( model, "mdl/goblin.md2" );
  }
}
