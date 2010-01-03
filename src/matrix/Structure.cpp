/*
 *  Structure.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "Structure.h"

#include "Synapse.h"

namespace oz
{

  Pool<Structure, 0, 256> Structure::pool;

  Structure::Structure( int index_, int bsp_, const Vec3& p_, Rotation rot_ ) :
      index( index_ ), bsp( bsp_ ), p( p_ ), rot( rot_ ), life( world.bsps[bsp]->life )
  {}

  Structure::Structure( int index_, int bsp_, InputStream* istream ) : index( index_ ), bsp( bsp_ )
  {
    readFull( istream );
  }

  void Structure::destroy()
  {
    synapse.genParts( 100, p, Vec3::zero(), 10.0f, Vec3( 0.4f, 0.4f, 0.4f ), 0.1f,
                      1.98f, 0.0f, 2.0f );
    synapse.remove( this );
  }

  void Structure::readFull( InputStream* istream )
  {
    p      = istream->readVec3();
    rot    = Rotation( istream->readByte() );
    life   = istream->readFloat();
  }

  void Structure::writeFull( OutputStream* ostream )
  {
    ostream->writeVec3( p );
    ostream->writeByte( byte( rot ) );
    ostream->writeFloat( life );
  }

}
