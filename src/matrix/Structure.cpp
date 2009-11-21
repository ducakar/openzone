/*
 *  Structure.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Structure.h"

namespace oz
{

  void Structure::readFull( InputStream *istream )
  {
    p      = istream->readVec3();
    flags  = istream->readInt();
    rot    = static_cast<Rotation>( istream->readByte() );
    life   = istream->readFloat();
  }

  void Structure::writeFull( OutputStream *ostream )
  {
    ostream->writeVec3( p );
    ostream->writeInt( flags );
    ostream->writeByte( static_cast<int>( rot ) );
    ostream->writeFloat( life );
  }

}
