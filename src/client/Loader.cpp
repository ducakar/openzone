/*
 *  Loader.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Loader.hpp"

#include "client/Context.hpp"
#include "client/Camera.hpp"

#include <ctime>
#include <AL/alut.h>

namespace oz
{
namespace client
{

  Loader loader;

  void Loader::cleanupRender()
  {
    if( tick % MODEL_CLEAR_INTERVAL == 0 ) {
      // remove unused BSPs
      for( int i = 0; i < translator.bsps.length(); ++i ) {
        Context::Resource<BSP*>& bsp = context.bsps[i];

        if( bsp.object != null ) {
          if( bsp.isUpdated ) {
            bsp.isUpdated = false;
          }
          else {
            delete bsp.object;
            bsp.object = null;
          }
        }
      }
      // remove unused models
      for( auto i = context.models.citer(); i.isValid(); ) {
        Model* model = *i;
        uint   key   = i.key();

        // we should advance now, so that we don't remove the element the iterator is pointing at
        ++i;

        if( model->flags & Model::UPDATED_BIT ) {
          model->flags &= ~Model::UPDATED_BIT;
        }
        else {
          context.models.exclude( key );
          delete model;
        }
      }
    }
  }

  void Loader::cleanupSound()
  {
    assert( alGetError() == AL_NO_ERROR );

    if( tick % SOURCE_CLEAR_INTERVAL ) {
      // remove stopped sources of non-continous sounds
      Context::Source* prev = null;
      Context::Source* src  = context.sources.first();

      while( src != null ) {
        Context::Source* next = src->next[0];

        ALint value;
        alGetSourcei( src->source, AL_SOURCE_STATE, &value );

        if( value != AL_PLAYING ) {
          alDeleteSources( 1, &src->source );
          context.sources.remove( src, prev );
          delete src;
        }
        else {
          prev = src;
        }
        src = next;
      }
    }

    assert( alGetError() == AL_NO_ERROR );

    if( tick % CONTSOURCE_CLEAR_INTERVAL == 0 ) {
      // remove continous sounds that are not played any more
      for( auto i = context.contSources.iter(); i.isValid(); ) {
        Context::ContSource* src = i;
        uint key = i.key();

        // we should advance now, so that we don't remove the element the iterator is pointing at
        ++i;

        if( src->isUpdated ) {
          src->isUpdated = false;
        }
        else {
          alSourceStop( src->source );
          alDeleteSources( 1, &src->source );
          context.contSources.exclude( key );
        }
      }
    }

    assert( alGetError() == AL_NO_ERROR );

    if( tick % AUDIO_CLEAR_INTERVAL == 0 ) {
      assert( alGetError() == AL_NO_ERROR );

      // remove unused Audio objects
      for( auto i = context.audios.citer(); i.isValid(); ) {
        Audio* audio = *i;
        uint   key   = i.key();

        // we should advance now, so that we don't remove the element the iterator is pointing at
        ++i;

        if( audio->flags & Audio::UPDATED_BIT ) {
          audio->flags &= ~Audio::UPDATED_BIT ;
        }
        else {
          context.audios.exclude( key );
          delete audio;
        }
      }

      assert( alGetError() == AL_NO_ERROR );

      // remove unused (no object audio uses it) buffers
      for( int i = 0; i < translator.sounds.length(); ++i ) {
        // first, only
        if( context.sounds[i].nUsers == 0 ) {
          context.sounds[i].nUsers = -2;
        }
        else if( context.sounds[i].nUsers == -2 ) {
          context.freeSound( i );
        }
      }
    }

    assert( alGetError() == AL_NO_ERROR );
  }

  void Loader::makeScreenshot()
  {
    uint* pixels = new uint[camera.width * camera.height * 4];
    char fileName[1024];
    time_t ct;
    struct tm t;

    ct = time( null );
    t = *localtime( &ct );

    snprintf( fileName, 1024, "%s/screenshot %04d-%02d-%02d %02d:%02d:%02d.bmp",
              config.get( "dir.rc", "" ),
              1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec );
    fileName[1023] = '\0';

    log.print( "Saving screenshot to '%s' ...", fileName );

    glReadPixels( 0, 0, camera.width, camera.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
    SDL_Surface* surf = SDL_CreateRGBSurfaceFrom( pixels, camera.width, camera.height, 32,
                                                  camera.width * 4,
                                                  0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 );
    // flip image
    for( int i = 0; i < camera.height / 2; ++i ) {
      for( int j = 0; j < camera.width; ++j ) {
        swap( pixels[i * camera.width + j],
              pixels[( camera.height - i - 1 ) * camera.width + j] );
      }
    }
    SDL_SaveBMP( surf, fileName );
    SDL_FreeSurface( surf );
    delete[] pixels;

    log.printEnd( "OK" );
  }

  void Loader::update()
  {
    tick = ( tick + 1 ) % TICK_CLEAR_PERIOD;
  }

  void Loader::cleanup()
  {
    cleanupRender();
    cleanupSound();
  }

  void Loader::load()
  {}

  void Loader::unload()
  {}

  void Loader::init()
  {
    log.print( "Initialising Loader ..." );

    tick = 0;

    log.printEnd( " OK" );
  }

  void Loader::free()
  {
    log.println( "Freeing Loader ... OK" );
  }

}
}
