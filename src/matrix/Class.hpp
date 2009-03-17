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

#define OZ_CLASS_READ_INT( name, default ) \
  if( config.contains( #name ) ) { \
    name = atoi( config[#name] ); \
  } \
  else { \
    logFile.println( "missing property " #name ); \
    name = ( default ); \
  }

#define OZ_CLASS_READ_FLOAT( name, default ) \
  if( config.contains( #name ) ) { \
    name = strtof( config[#name], null ); \
  } \
  else { \
    logFile.println( "missing property " #name ); \
    name = ( default ); \
  }

#define OZ_CLASS_READ_STRING( name, default ) \
  if( config.contains( #name ) ) { \
    name = config[#name]; \
  } \
  else { \
    logFile.println( "missing property " #name ); \
    name = ( default ); \
  }

namespace oz
{

  struct Class
  {

  };

}
