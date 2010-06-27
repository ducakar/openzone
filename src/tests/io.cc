/*
 *  io.cc
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#include "matrix/io.hh"

#include <SDL_main.h>

OZ_IMPORT()

int main( int, char** )
{
  Buffer buffer( 1024 );
  OutputStream os = buffer.outputStream();
  InputStream is = buffer.inputStream();

  os.writeVec3( Vec3( 1.5f, 0.7f, 4.9f ) );
  os.writeInt( 42 );
  os.writeFloat( 1.7f );
  os.writeString( "drek" );
  os.writeInt( 43 );
  os.writeVec3( Vec3( 1.51f, 0.71f, 4.91f ) );
  os.writeFloat( 1.71f );

  Vec3 v1 = is.readVec3();
  int i1 = is.readInt();
  float f1 = is.readFloat();
  String s1 = is.readString();
  int i2 = is.readInt();
  Vec3 v2 = is.readVec3();
  float f2 = is.readFloat();

  printf( "(%g %g %g) %d %g %s %d (%g %g %g) %g\n", v1.x, v1.y, v1.z, i1, f1, s1.cstr(), i2, v2.x, v2.y, v2.z, f2 );
  return 0;
}
