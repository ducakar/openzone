/*
 *  Structure.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Structure.h"

namespace oz
{

  void Structure::readFull( InputStream *istream )
  {
    p      = istream->readVec3();
    rot    = static_cast<Rotation>( istream->readByte() );
    life   = istream->readFloat();
  }

  void Structure::writeFull( OutputStream *ostream )
  {
    ostream->writeVec3( p );
    ostream->writeByte( static_cast<int>( rot ) );
    ostream->writeFloat( life );
  }

}
