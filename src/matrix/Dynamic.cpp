/*
 *  Dynamic.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.hpp"

#include "matrix/Dynamic.hpp"

namespace oz
{

  Pool<Dynamic> Dynamic::pool;

  void Dynamic::readFull( InputStream* istream )
  {
    Object::readFull( istream );

    parent     = istream->readInt();
    velocity   = istream->readVec4();
    momentum   = istream->readVec4();
    floor      = istream->readVec4();
    lower      = istream->readInt();
    depth      = istream->readFloat();
  }

  void Dynamic::writeFull( OutputStream* ostream ) const
  {
    Object::writeFull( ostream );

    ostream->writeInt( parent );
    ostream->writeVec4( velocity );
    ostream->writeVec4( momentum );
    ostream->writeVec4( floor );
    ostream->writeInt( lower );
    ostream->writeFloat( depth );
  }

  void Dynamic::readUpdate( InputStream* istream )
  {
    Object::readUpdate( istream );

    p        = istream->readVec4();
    velocity = istream->readVec4();
    momentum = istream->readVec4();
  }

  void Dynamic::writeUpdate( OutputStream* ostream ) const
  {
    Object::writeUpdate( ostream );

    ostream->writeVec4( p );
    ostream->writeFloat( life );
    ostream->writeVec4( velocity );
    ostream->writeVec4( momentum );
  }

}
