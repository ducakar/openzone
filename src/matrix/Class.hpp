/*
 *  Class.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "Object.hpp"

#define OZ_CLASS_READ_INT( clazz, name, default ) \
  if( config.contains( #name ) ) { \
    clazz->name = atoi( config[#name] ); \
  } \
  else { \
    logFile.println( "missing property " #name ); \
    clazz->name = ( default ); \
  }

#define OZ_CLASS_READ_FLOAT( clazz, name, default ) \
  if( config.contains( #name ) ) { \
    clazz->name = strtof( config[#name], null ); \
  } \
  else { \
    logFile.println( "missing property " #name ); \
    clazz->name = ( default ); \
  }

#define OZ_CLASS_READ_STRING( clazz, name, default ) \
  if( config.contains( #name ) ) { \
    clazz->name = config[#name]; \
  } \
  else { \
    logFile.println( "missing property " #name ); \
    clazz->name = ( default ); \
  }

namespace oz
{

  struct Class
  {
    typedef Class *( *InitFunc )( Config *config );

    virtual Object *create( const Vec3 &pos ) = 0;
  };

}
