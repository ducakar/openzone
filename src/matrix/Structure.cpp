/*
 *  Structure.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Structure.hpp"

#include "matrix/Synapse.hpp"

namespace oz
{

  const float Structure::DAMAGE_THRESHOLD = 400.0f;

  Pool<Structure, 0, 256> Structure::pool;

  Structure::Structure( int index_, int bsp_, const Vec3& p_, Rotation rot_ ) :
      index( index_ ), bsp( bsp_ ), p( p_ ), rot( rot_ ), life( orbis.bsps[bsp]->life ),
      entities( orbis.bsps[bsp]->nEntities )
  {
    for( int i = 0; i < entities.length(); ++i ) {
      Entity& entity = entities[i];

      entity.offset.setZero();
      entity.state = Entity::CLOSED;
      entity.timer = 0.0f;
    }
  }

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

  void Structure::update()
  {
    for( int i = 1; i < entities.length(); ++i ) {
      Entity& entity = entities[i];

      if( entity.state == Entity::OPENED ) {
        entity.timer += Timer::TICK_TIME;

        if( entity.timer >= 5.0f ) {
          entity.state = Entity::CLOSING;
          entity.timer = 0.0f;
        }
      }
      else if( entity.state == Entity::OPENING ) {
        entity.timer += Timer::TICK_TIME;
        entity.offset.x += 0.01f;

        if( entity.timer >= 5.0f ) {
          entity.state = Entity::OPENED;
          entity.timer = 0.0f;
        }
      }
      else if( entity.state == Entity::CLOSING ) {
        entity.timer += Timer::TICK_TIME;
        entity.offset.x -= 0.01f;

        if( entity.timer >= 5.0f ) {
          entity.state = Entity::CLOSED;
          entity.timer = 0.0f;
        }
      }
      else {
        entity.timer += Timer::TICK_TIME;

        if( entity.timer >= 5.0f ) {
          entity.state = Entity::OPENING;
          entity.timer = 0.0f;
        }
      }
    }
  }

  void Structure::readFull( InputStream* istream )
  {
    p    = istream->readVec3();
    rot  = Rotation( istream->readChar() );
    life = istream->readFloat();

    entities( istream->readInt() );
    for( int i = 0; i < entities.length(); ++i ) {
      entities[i].offset = istream->readVec3();
      entities[i].timer = istream->readFloat();
      entities[i].state = Entity::State( istream->readChar() );
    }
  }

  void Structure::writeFull( OutputStream* ostream )
  {
    ostream->writeVec3( p );
    ostream->writeChar( char( rot ) );
    ostream->writeFloat( life );

    ostream->writeInt( entities.length() );
    for( int i = 0; i < entities.length(); ++i ) {
      ostream->writeVec3( entities[i].offset );
      ostream->writeFloat( entities[i].timer );
      ostream->writeChar( entities[i].state );
    }
  }

}
