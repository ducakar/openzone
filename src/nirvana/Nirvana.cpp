/*
 *  Nirvana.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Nirvana.h"

#include "matrix/Matrix.h"
#include "matrix/BotClass.h"

#include "RandomMind.h"

#define OZ_REGISTER_MINDCLASS( name ) \
  mindClasses.add( #name, MindCtor( &name##Mind::create, &name##Mind::read ) )

namespace oz
{

  Nirvana nirvana;

  void Nirvana::sync()
  {
    // remove minds of removed bots
    for( typeof( minds.iterator() ) i = minds.iterator(); !i.isPassed(); ) {
      Mind *mind = i;
      ++i;

      if( world.objects[mind->botIndex] == null ) {
        minds.remove( mind );
        delete mind;
      }
    }
    // add minds for new bots
    foreach( obj, synapse.putObjects.iterator() ) {
      if( ( *obj )->flags & Object::BOT_BIT ) {
        Bot *bot = (Bot*) *obj;

        if( ~bot->state & Bot::PLAYER_BIT ) {
          BotClass *clazz = (BotClass*) bot->type;

          if( mindClasses.contains( clazz->mindType ) ) {
            minds << mindClasses.cachedValue().create( bot->index );
          }
          else {
            throw Exception( 0, "Invalid mind type" );
          }
        }
      }
    }
  }

  void Nirvana::update()
  {
    foreach( mind, minds.iterator() ) {
      mind->update();
    }
  }

  int Nirvana::run( void* )
  {
    while( nirvana.isAlive ) {
      nirvana.sync();

      // update minds
      nirvana.update();

      // notify matrix it can proceed changing the world
      SDL_SemPost( matrix.semaphore );
      // wait until world has changed
      SDL_SemWait( nirvana.semaphore );
    }
    return 0;
  }

  void Nirvana::init()
  {
    log.print( "Initializing Nirvana ..." );

    OZ_REGISTER_MINDCLASS();
    OZ_REGISTER_MINDCLASS( Random );

    semaphore = SDL_CreateSemaphore( 0 );

    log.printEnd( " OK" );
  }

  void Nirvana::free()
  {
    log.print( "Freeing Nirvana ..." );

    SDL_DestroySemaphore( semaphore );

    minds.free();
    mindClasses.clear();

    log.printEnd( " OK" );
  }

  void Nirvana::load( InputStream *istream )
  {
    log.print( "Loading Nirvana ..." );

    if( istream != null ) {
      read( istream );
    }

    log.printEnd( " OK" );
  }

  void Nirvana::unload( OutputStream *ostream )
  {
    log.print( "Unloading Nirvana ..." );

    if( ostream != null ) {
      write( ostream );
    }
    minds.free();

    log.printEnd( " OK" );
  }

  void Nirvana::start()
  {
    log.print( "Starting Nirvana thread ..." );

    isAlive = true;
    thread = SDL_CreateThread( run, null );

    log.printEnd( " OK" );
  }

  void Nirvana::stop()
  {
    log.print( "Stopping Nirvana thread ..." );

    isAlive = false;

    SDL_SemPost( semaphore );
    SDL_WaitThread( thread, null );

    thread = null;

    log.printEnd( " OK" );
  }

  void Nirvana::read( InputStream *istream )
  {
    String typeName;
    int nMinds = istream->readInt();

    for( int i = 0; i < nMinds; i++ ) {
      istream->readString( typeName );
      minds << mindClasses[typeName].read( istream );
    }
  }

  void Nirvana::write( OutputStream *ostream ) const
  {
    ostream->writeInt( minds.length() );

    foreach( mind, minds.iterator() ) {
      ostream->writeString( mind->type() );
      mind->write( ostream );
    }
  }

}
