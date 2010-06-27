/*
 *  Dynamic.cc
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#include "matrix/Dynamic.hh"

namespace oz
{

  Pool<Dynamic> Dynamic::pool;

  void Dynamic::readFull( InputStream* istream )
  {
    Object::readFull( istream );

    parent     = istream->readInt();
    velocity   = istream->readVec3();
    momentum   = istream->readVec3();
    floor      = istream->readVec3();
    lower      = istream->readInt();
    depth      = istream->readFloat();
  }

  void Dynamic::writeFull( OutputStream* ostream ) const
  {
    Object::writeFull( ostream );

    ostream->writeInt( parent );
    ostream->writeVec3( velocity );
    ostream->writeVec3( momentum );
    ostream->writeVec3( floor );
    ostream->writeInt( lower );
    ostream->writeFloat( depth );
  }

  void Dynamic::readUpdate( InputStream* istream )
  {
    Object::readUpdate( istream );

    p        = istream->readVec3();
    velocity = istream->readVec3();
    momentum = istream->readVec3();
  }

  void Dynamic::writeUpdate( OutputStream* ostream ) const
  {
    Object::writeUpdate( ostream );

    ostream->writeVec3( p );
    ostream->writeFloat( life );
    ostream->writeVec3( velocity );
    ostream->writeVec3( momentum );
  }

}
