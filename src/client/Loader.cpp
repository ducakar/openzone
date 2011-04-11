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
#include "client/SMM.hpp"
#include "client/MD2.hpp"
#include "client/MD3.hpp"

#include <ctime>
#include <AL/al.h>

namespace oz
{
namespace client
{

  Loader loader;

  void Loader::cleanupRender()
  {
    hard_assert( glGetError() == GL_NO_ERROR );

    // delete models of removed objects
    for( auto i = context.models.citer(); i.isValid(); ) {
      auto model = i;
      ++i;

      // We can afford to do this as orbis.objects[key] will remain null at least one whole tick
      // after the object has been removed (because matrix also needs to clear references to this
      // object).
      if( orbis.objects[ model.key() ] == null ) {
        delete model.value();
        context.models.exclude( model.key() );
      }
    }

    hard_assert( glGetError() == GL_NO_ERROR );

    if( tick % BSP_CLEAR_INTERVAL == 0 ) {
      // remove unused BSPs
      for( int i = 0; i < translator.bsps.length(); ++i ) {
        Context::Resource<BSP*>& bsp = context.bsps[i];

        if( bsp.object != null ) {
          if( bsp.nUsers != 0 ) {
            bsp.nUsers = 0;
          }
          else {
            delete bsp.object;
            bsp.object = null;
          }
        }
      }
    }

    hard_assert( glGetError() == GL_NO_ERROR );

    if( tick % MODEL_CLEAR_INTERVAL == 0 ) {
      // remove unused models
      for( auto i = context.models.citer(); i.isValid(); ) {
        auto model = i;
        ++i;

        if( model.value()->flags & Model::UPDATED_BIT ) {
          model.value()->flags &= ~Model::UPDATED_BIT;
        }
        else {
          context.models.exclude( model.key() );
          delete model.value();
        }
      }
    }

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Loader::cleanupSound()
  {
    hard_assert( alGetError() == AL_NO_ERROR );

    // remove audio models of removed objects
    for( auto i = context.audios.citer(); i.isValid(); ) {
      auto audio = i;
      ++i;

      // We can afford to do this as orbis.objects[key] will remain null at least one whole tick
      // after the object has been removed (because matrix also needs to clear references to this
      // object).
      if( orbis.objects[ audio.key() ] == null ) {
        delete audio.value();
        context.audios.exclude( audio.key() );
      }
    }

    hard_assert( alGetError() == AL_NO_ERROR );

    // remove continous sounds that are not played any more
    for( auto i = context.bspSources.iter(); i.isValid(); ) {
      auto src = i;
      ++i;

      if( src->isUpdated ) {
        src->isUpdated = false;
      }
      else {
        alDeleteSources( 1, &src->id );
        context.removeBSPSource( src, src.key() );
      }
    }

    for( auto i = context.objSources.iter(); i.isValid(); ) {
      auto src = i;
      ++i;

      if( src->isUpdated ) {
        src->isUpdated = false;
      }
      else {
        alDeleteSources( 1, &src->id );
        context.removeObjSource( src, src.key() );
      }
    }

    hard_assert( alGetError() == AL_NO_ERROR );

    if( tick % SOURCE_CLEAR_INTERVAL ) {
      // remove stopped sources of non-continous sounds
      Context::Source* prev = null;
      Context::Source* src  = context.sources.first();

      while( src != null ) {
        Context::Source* next = src->next[0];

        ALint value;
        alGetSourcei( src->id, AL_SOURCE_STATE, &value );

        if( value != AL_PLAYING ) {
          alDeleteSources( 1, &src->id );
          context.removeSource( src, prev );
        }
        else {
          prev = src;
        }
        src = next;
      }
    }

    hard_assert( alGetError() == AL_NO_ERROR );

    if( tick % AUDIO_CLEAR_INTERVAL == 0 ) {
      hard_assert( alGetError() == AL_NO_ERROR );

      // remove unused Audio objects
      for( auto i = context.audios.citer(); i.isValid(); ) {
        auto audio = i;
        ++i;

        if( audio.value()->flags & Audio::UPDATED_BIT ) {
          audio.value()->flags &= ~Audio::UPDATED_BIT ;
        }
        else {
          context.audios.exclude( audio.key() );
          delete audio.value();
        }
      }
    }

    hard_assert( alGetError() == AL_NO_ERROR );
  }

  void Loader::update()
  {
    tick = ( tick + 1 ) % TICK_CLEAR_PERIOD;

    // BSP
    for( int i = 0; i < translator.bsps.length(); ++i ) {
      BSP* bsp = context.bsps[i].object;

      if( bsp != null && !bsp->isLoaded ) {
        bsp->load();
      }
    }

    // SMM
    for( auto i = context.smms.iter(); i.isValid(); ) {
      auto j = i;

      ++i;

      if( !j->object->isLoaded ) {
        j->object->load();
      }
      else if( j->nUsers == 0 ) {
        delete j->object;
        context.smms.exclude( j.key() );;
      }
    }

    // MD2
    for( auto i = context.md2s.iter(); i.isValid(); ) {
      auto j = i;

      ++i;

      if( !j->object->isLoaded ) {
        j->object->load();
      }
      else if( j->nUsers == 0 ) {
        delete j->object;
        context.md2s.exclude( j.key() );;
      }
    }

    // MD3
    for( auto i = context.md3s.iter(); i.isValid(); ) {
      auto j = i;

      ++i;

      if( !j->object->isLoaded ) {
        j->object->load();
      }
      else if( j->nUsers == 0 ) {
        delete j->object;
        context.md3s.exclude( j.key() );;
      }
    }
  }

  void Loader::cleanup()
  {
    cleanupRender();
    cleanupSound();
  }

  void Loader::makeScreenshot()
  {
    uint* pixels = new uint[camera.width * camera.height * 4];
    char fileName[1024];
    time_t ct;
    struct tm t;

    ct = time( null );
    t = *localtime( &ct );

    snprintf( fileName, 1024, "%s/screenshot %d.%d.%04d %02d:%02d:%02d.bmp",
              config.get( "dir.rc", "" ),
              t.tm_mday, 1 + t.tm_mon, 1900 + t.tm_year, t.tm_hour, t.tm_min, t.tm_sec );
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

  void Loader::init()
  {
    tick = 0;
  }

  void Loader::free()
  {}

}
}
