/*
 *  Render.cpp
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Render.hpp"

#include "matrix/Matrix.hpp"
#include "matrix/Physics.hpp"
#include "matrix/BotClass.hpp"

#include "client/Frustum.hpp"
#include "client/Colours.hpp"
#include "client/Shape.hpp"

#include "client/Water.hpp"
#include "client/Sky.hpp"
#include "client/Terra.hpp"
#include "client/BSP.hpp"

#include "client/OBJModel.hpp"
#include "client/OBJVehicleModel.hpp"
#include "client/MD2StaticModel.hpp"
#include "client/MD2Model.hpp"
#include "client/MD2WeaponModel.hpp"
#include "client/MD3StaticModel.hpp"
#include "client/ExplosionModel.hpp"

#include <ctime>
#include <SDL_opengl.h>

namespace oz
{
namespace client
{

  Render render;

  const float Render::WIDE_CULL_FACTOR = 6.0f;
  const float Render::CELL_WIDE_RADIUS = Cell::RADIUS + AABB::MAX_DIM * WIDE_CULL_FACTOR;

  const float Render::NIGHT_FOG_COEFF = 2.0f;
  const float Render::NIGHT_FOG_DIST = 0.3f;
  const float Render::WATER_VISIBILITY = 8.0f;

  const float Render::STAR_SIZE = 1.0f / 400.0f;

  void Render::scheduleCell( int cellX, int cellY )
  {
    Cell& cell = orbis.cells[cellX][cellY];

    for( int i = 0; i < cell.structs.length(); ++i ) {
      Structure* str = orbis.structs[ cell.structs[i] ];

      if( !drawnStructures.get( cell.structs[i] ) && frustum.isVisible( *str ) ) {
        drawnStructures.set( cell.structs[i] );
        structures.add( str );
      }
    }

    for( const Object* obj = cell.firstObject; obj != null; obj = obj->next[0] ) {
      if( obj->flags & Object::NO_DRAW_BIT ) {
        continue;
      }
      bool isVisible =
          ( obj->flags & Object::WIDE_CULL_BIT ) ?
              frustum.isVisible( *obj, WIDE_CULL_FACTOR ) :
              frustum.isVisible( *obj );

      if( isVisible ) {
        ObjectEntry entry( ( obj->p - camera.p ).sqL(), obj );

        if( obj->flags & Object::DELAYED_DRAW_BIT ) {
          delayedObjects.add( entry );
        }
        else {
          objects.add( entry );
        }
      }
    }

    for( const Particle* part = cell.firstPart; part != null; part = part->next[0] ) {
      if( frustum.isVisible( part->p, particleRadius ) ) {
        particles.add( part );
      }
    }
  }

  void Render::drawOrbis()
  {
    assert( glGetError() == GL_NO_ERROR );
    assert( !glIsEnabled( GL_TEXTURE_2D ) );

    // clear colour, visibility, fog
    if( isUnderWater ) {
      visibility = sky.ratio * waterDayVisibility + sky.ratio_1 * waterNightVisibility;

      glClearColor( Colours::water[0], Colours::water[1], Colours::water[2], Colours::water[3] );
      glFogfv( GL_FOG_COLOR, Colours::water );
      glFogf( GL_FOG_END, visibility );
    }
    else {
      visibility = sky.ratio * dayVisibility + sky.ratio_1 * nightVisibility;

      glClearColor( Colours::sky[0], Colours::sky[1], Colours::sky[2], Colours::sky[3] );
      glFogfv( GL_FOG_COLOR, Colours::sky );
      glFogf( GL_FOG_END, visibility );
    }

    // frustum
    Span span;
    frustum.update( visibility );
    frustum.getExtrems( span, camera.p );

    span.minX = max( span.minX - 2, 0 );
    span.maxX = min( span.maxX + 2, Orbis::MAX - 1 );
    span.minY = max( span.minY - 2, 0 );
    span.maxY = min( span.maxY + 2, Orbis::MAX - 1 );

    sky.update();
    water.update();

    // drawnStructures
    if( drawnStructures.length() < orbis.structs.length() ) {
      drawnStructures.setSize( orbis.structs.length() );
    }
    drawnStructures.clearAll();

    float minXCenter = float( span.minX - Orbis::MAX / 2 ) * Cell::SIZE + Cell::SIZE / 2.0f;
    float minYCenter = float( span.minY - Orbis::MAX / 2 ) * Cell::SIZE + Cell::SIZE / 2.0f;

    float x = minXCenter;
    for( int i = span.minX; i <= span.maxX; ++i, x += Cell::SIZE ) {
      float y = minYCenter;

      for( int j = span.minY; j <= span.maxY; ++j, y += Cell::SIZE ) {
        if( frustum.isVisible( x, y, CELL_WIDE_RADIUS  ) ) {
          scheduleCell( i, j );
        }
      }
    }

    // clear buffer
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    assert( !glIsEnabled( GL_TEXTURE_2D ) );
    assert( glIsEnabled( GL_BLEND ) );

    // camera transformation
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( camera.angle, camera.aspect, camera.minDist, visibility );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
    glMultMatrixf( camera.rotTMat );

    if( !isUnderWater ) {
      sky.draw();
    }

    glTranslatef( -camera.p.x, -camera.p.y, -camera.p.z );

    // lighting
    glLightfv( GL_LIGHT0, GL_POSITION, sky.lightDir );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, Colours::diffuse );
    glLightfv( GL_LIGHT0, GL_AMBIENT, Colours::ambient );

    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_FOG );
    glEnable( GL_LIGHTING );
    glDisable( GL_BLEND );
    glEnable( GL_TEXTURE_2D );

    wasUnderWater = isUnderWater;
    isUnderWater  = camera.p.z < 0.0f;

    terra.radius = frustum.radius;
    terra.draw();

    // draw structures
    BSP::beginRender();

    for( int i = 0; i < structures.length(); ++i ) {
      const Structure* str = structures[i];

      if( bsps[str->bsp] == null ) {
        bsps[str->bsp] = new BSP( str->bsp );
      }

      int waterFlags = bsps[str->bsp]->fullDraw( str );
      bsps[str->bsp]->isUpdated = true;

      if( waterFlags & BSP::IN_WATER_BRUSH ) {
        isUnderWater = true;
      }
      if( waterFlags & BSP::DRAW_WATER ) {
        waterStructures.add( str );
      }
    }

    BSP::endRender();

    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );

    // draw (non-delayed) objects
    objects.sort();

    for( int i = 0; i < objects.length(); ++i ) {
      const Object* obj = objects[i].obj;

      if( obj->index == camera.tagged ) {
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colours::TAG );
      }

      glPushMatrix();
      glTranslatef( obj->p.x, obj->p.y, obj->p.z );

      drawModel( obj, null );

      glPopMatrix();

      if( obj->index == camera.tagged ) {
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colours::BLACK );
      }
    }

    assert( !glIsEnabled( GL_BLEND ) );

    // draw particles
    glEnable( GL_COLOR_MATERIAL );
    glDisable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );

    for( int i = 0; i < particles.length(); ++i ) {
      const Particle* part = particles[i];

      glPushMatrix();
      glTranslatef( part->p.x, part->p.y, part->p.z );

      shape.draw( part );

      glPopMatrix();
    }

    assert( glGetError() == GL_NO_ERROR );

    glColor4fv( Colours::WHITE );
    glEnable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );

    // draw delayed objects
    glDisable( GL_COLOR_MATERIAL );

    for( int i = 0; i < delayedObjects.length(); ++i ) {
      const Object* obj = delayedObjects[i].obj;

      if( obj->index == camera.tagged ) {
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colours::TAG );
      }

      glPushMatrix();
      glTranslatef( obj->p.x, obj->p.y, obj->p.z );

      drawModel( obj, null );

      glPopMatrix();

      if( obj->index == camera.tagged ) {
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colours::BLACK );
      }
    }

    assert( !glIsEnabled( GL_BLEND ) );
    assert( glIsEnabled( GL_TEXTURE_2D ) );
    glEnable( GL_BLEND );

    // draw structures' water
    BSP::beginRender();

    for( int i = 0; i < waterStructures.length(); ++i ) {
      const Structure* str = waterStructures[i];

      bsps[str->bsp]->fullDrawWater( str );
    }

    BSP::endRender();

    terra.drawWater();

    glDisable( GL_TEXTURE_2D );
    glDisable( GL_LIGHTING );

    if( showAim ) {
      Vec3 move = camera.at * 32.0f;
      collider.translate( camera.p, move, camera.botObj );
      move *= collider.hit.ratio;

      glColor3f( 0.0f, 1.0f, 0.0f );
      shape.drawBox( AABB( camera.p + move, Vec3( 0.05f, 0.05f, 0.05f ) ) );
    }

    glEnable( GL_BLEND );

    if( drawAABBs ) {
      glEnable( GL_COLOR_MATERIAL );

      for( int i = 0; i < objects.length(); ++i ) {
        glColor4fv( ( objects[i].obj->flags & Object::SOLID_BIT ) ?
            Colours::CLIP_AABB : Colours::NOCLIP_AABB );
        shape.drawBox( *objects[i].obj );
      }

      glColor4fv( Colours::STRUCTURE_AABB );

      for( int i = 0; i < structures.length(); ++i ) {
        shape.drawBox( structures[i]->toAABB() );
      }

      glColor4fv( Colours::WHITE );
      glDisable( GL_COLOR_MATERIAL );
    }

    structures.clear();
    waterStructures.clear();
    objects.clear();
    delayedObjects.clear();
    particles.clear();

    glDisable( GL_FOG );

    glDisable( GL_DEPTH_TEST );
    glColor4fv( Colours::WHITE );

    assert( glGetError() == GL_NO_ERROR );
  }

  void Render::drawCommon()
  {
    ui::ui.draw();

    assert( !glIsEnabled( GL_TEXTURE_2D ) );

    if( doScreenshot ) {
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

      doScreenshot = false;
      log.printEnd( "OK" );
    }

    SDL_GL_SwapBuffers();

    // cleanups
    if( clearCount >= CLEAR_INTERVAL ) {
      // remove unused BSPs
      for( int i = 0; i < bsps.length(); ++i ) {
        if( bsps[i] != null ) {
          if( bsps[i]->isUpdated ) {
            bsps[i]->isUpdated = false;
          }
          else {
            delete bsps[i];
            bsps[i] = null;
          }
        }
      }
      // remove unused models
      for( auto i = models.citer(); i.isValid(); ) {
        Model* model = *i;
        uint   key   = i.key();

        // we should advance now, so that we don't remove the element the iterator is pointing at
        ++i;

        if( model->flags & Model::UPDATED_BIT ) {
          model->flags &= ~Model::UPDATED_BIT;
        }
        else {
          models.exclude( key );
          delete model;
        }
      }
      clearCount = 0;
    }
    else {
      clearCount += timer.frameMillis;
    }
  }

  void Render::sync()
  {
    for( auto i = models.citer(); i.isValid(); ) {
      Model* model = i.value();
      uint   key   = i.key();
      ++i;

      if( orbis.objects[key] == null ) {
        delete model;
        models.exclude( key );
      }
    }
  }

  void Render::update()
  {
    drawOrbis();
    drawCommon();
  }

  void Render::init()
  {
    doScreenshot = false;
    clearCount   = 0;

    log.println( "Initialising Graphics {" );
    log.indent();

    DArray<String> extensions;
    String sExtensions = reinterpret_cast<const char*>( glGetString( GL_EXTENSIONS ) );
    sExtensions.trim().split( ' ', &extensions );

    log.println( "OpenGL vendor: %s", glGetString( GL_VENDOR ) );
    log.println( "OpenGL renderer: %s", glGetString( GL_RENDERER ) );
    log.println( "OpenGL version: %s", glGetString( GL_VERSION ) );
    log.println( "OpenGL extensions {" );
    log.indent();
    foreach( extension, extensions.citer() ) {
      log.println( "%s", extension->cstr() );
    }

#ifdef OZ_WINDOWS
    glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>( SDL_GL_GetProcAddress( "glActiveTexture" ) );
    glClientActiveTexture = reinterpret_cast<PFNGLCLIENTACTIVETEXTUREPROC>( SDL_GL_GetProcAddress( "glClientActiveTexture" ) );
    glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>( SDL_GL_GetProcAddress( "glGenBuffers" ) );
    glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>( SDL_GL_GetProcAddress( "glDeleteBuffers" ) );
    glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>( SDL_GL_GetProcAddress( "glBindBuffer" ) );
    glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>( SDL_GL_GetProcAddress( "glBufferData" ) );
#endif

    log.unindent();
    log.println( "}" );

    glDepthFunc( GL_LEQUAL );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    camera.init();
    ui::ui.init();

    SDL_GL_SwapBuffers();

    assert( glGetError() == GL_NO_ERROR );

    log.unindent();
    log.println( "}" );
  }

  void Render::free()
  {
    log.println( "Shutting down Graphics {" );
    log.indent();

    ui::ui.free();

    log.unindent();
    log.println( "}" );
  }

  void Render::load()
  {
    log.println( "Loading Graphics {" );
    log.indent();

    assert( glGetError() == GL_NO_ERROR );

    dayVisibility        = config.getSet( "render.dayVisibility",        300.0f );
    nightVisibility      = config.getSet( "render.nightVisibility",      100.0f );
    waterDayVisibility   = config.getSet( "render.waterDayVisibility",   8.0f );
    waterNightVisibility = config.getSet( "render.waterNightVisibility", 4.0f );
    particleRadius       = config.getSet( "render.particleRadius",       0.5f );
    drawAABBs            = config.getSet( "render.drawAABBs",            false );
    showAim              = config.getSet( "render.showAim",              false );

    frustum.init( camera.angle, camera.aspect, camera.maxDist );
    water.init();
    shape.load();
    sky.load();
    terra.load();
    MD2::init();

    for( int i = 0; i < translator.bsps.length(); ++i ) {
      bsps.add( null );
    }

    glEnable( GL_POINT_SMOOTH );
    glPointSize( float( camera.height ) * STAR_SIZE );

    // fog
    glFogi( GL_FOG_MODE, GL_LINEAR );
    glFogf( GL_FOG_START, 0.0f );

    // lighting
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
    glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, Colours::GLOBAL_AMBIENT );
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::WHITE );
    glEnable( GL_LIGHT0 );

    glEnable( GL_BLEND );

    log.unindent();
    log.println( "}" );
  }

  void Render::unload()
  {
    log.println( "Unloading Graphics {" );
    log.indent();

    terra.unload();
    sky.unload();
    shape.unload();

    bsps.free();
    bsps.trim();
    drawnStructures.setSize( 0 );

    models.free();

    OBJModel::pool.free();
    OBJVehicleModel::pool.free();
    MD2StaticModel::pool.free();
    MD2Model::pool.free();
    MD2WeaponModel::pool.free();
    MD3StaticModel::pool.free();
    ExplosionModel::pool.free();

    log.unindent();
    log.println( "}" );
  }

}
}
