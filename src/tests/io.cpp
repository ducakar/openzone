/*
 *  io.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "matrix/io.h"

using namespace oz;

int main()
{
  Buffer buffer( 1024 );
  OutputStream os = buffer.outputStream();
  InputStream is = buffer.inputStream();

  os.writeVec3( Vec3( 1.5, 0.7, 4.9 ) );
  os.writeInt( 42 );
  os.writeFloat( 1.7 );
  os.writeString( "drek" );
  os.writeInt( 43 );
  os.writeVec3( Vec3( 1.51, 0.71, 4.91 ) );
  os.writeFloat( 1.71 );

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