/*
 *  Render.cpp
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "Render.h"

#include "matrix/Matrix.h"
#include "matrix/Physics.h"
#include "matrix/BotClass.h"

#include "Frustum.h"
#include "Colors.h"
#include "Shape.h"

#include "Water.h"
#include "Sky.h"
#include "Terrain.h"
#include "BSP.h"

#include "OBJModel.h"
#include "OBJVehicleModel.h"
#include "MD2StaticModel.h"
#include "MD2Model.h"
#include "MD2WeaponModel.h"
#include "MD3StaticModel.h"
#include "ExplosionModel.h"

#include <ctime>
#include <GL/glu.h>
#include <GL/glext.h>

#ifdef OZ_MINGW32
static PFNGLACTIVETEXTUREPROC glActiveTexture = null;
#endif

namespace oz
{
namespace client
{

  Render render;

  const float Render::CELL_WIDE_RADIUS = Cell::RADIUS + AABB::MAX_DIM * WIDE_CULL_FACTOR;

  const float Render::NIGHT_FOG_COEFF = 2.0f;
  const float Render::NIGHT_FOG_DIST = 0.3f;
  const float Render::WATER_VISIBILITY = 8.0f;

  const float Render::STAR_SIZE = 1.0f / 400.0f;

  void Render::scheduleCell( int cellX, int cellY )
  {
    Cell& cell = world.cells[cellX][cellY];

    for( int i = 0; i < cell.structs.length(); ++i ) {
      Structure* str = world.structs[ cell.structs[i] ];

      if( !drawnStructures.get( cell.structs[i] ) && frustum.isVisible( *str ) ) {
        drawnStructures.set( cell.structs[i] );
        structures << str;
      }
    }

    foreach( obj, cell.objects.begin() ) {
      if( obj->flags & Object::NO_DRAW_BIT ) {
        continue;
      }
      bool isVisible =
          ( obj->flags & Object::WIDE_CULL_BIT ) ?
              frustum.isVisible( *obj * WIDE_CULL_FACTOR ) :
              frustum.isVisible( *obj );

      if( isVisible ) {
        ObjectEntry entry( ( obj->p - camera.p ).sqL(), obj );

        if( obj->flags & Object::DELAYED_DRAW_BIT ) {
          delayedObjects << entry;
        }
        else {
          objects << entry;
        }
      }
    }

    foreach( part, cell.parts.begin() ) {
      if( frustum.isVisible( part->p, particleRadius ) ) {
        particles << part;
      }
    }
  }

  void Render::drawWorld()
  {
    assert( glGetError() == GL_NO_ERROR );
    assert( !glIsEnabled( GL_TEXTURE_2D ) );

    // clear color, visibility, fog
    if( isUnderWater ) {
      visibility = sky.ratio * waterDayVisibility + sky.ratio_1 * waterNightVisibility;

      glClearColor( Colors::water[0], Colors::water[1], Colors::water[2], Colors::water[3] );
      glFogfv( GL_FOG_COLOR, Colors::water );
      glFogf( GL_FOG_END, visibility );
    }
    else {
      visibility = sky.ratio * dayVisibility + sky.ratio_1 * nightVisibility;

      glClearColor( Colors::sky[0], Colors::sky[1], Colors::sky[2], Colors::sky[3] );
      glFogfv( GL_FOG_COLOR, Colors::sky );
      glFogf( GL_FOG_END, visibility );
    }

    // frustum
    Span span;
    frustum.update( visibility );
    frustum.getExtrems( span, camera.p );

    span.minX = max( span.minX - 2, 0 );
    span.maxX = min( span.maxX + 2, World::MAX - 1 );
    span.minY = max( span.minY - 2, 0 );
    span.maxY = min( span.maxY + 2, World::MAX - 1 );

    sky.update();
    water.update();

    // drawnStructures
    if( drawnStructures.length() < world.structs.length() ) {
      drawnStructures.setSize( world.structs.length() );
    }
    drawnStructures.clearAll();

    float minXCenter = float( span.minX - World::MAX / 2 ) * Cell::SIZE + Cell::SIZE / 2.0f;
    float minYCenter = float( span.minY - World::MAX / 2 ) * Cell::SIZE + Cell::SIZE / 2.0f;

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
    glLightfv( GL_LIGHT0, GL_DIFFUSE, Colors::diffuse );
    glLightfv( GL_LIGHT0, GL_AMBIENT, Colors::ambient );

    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_FOG );
    glEnable( GL_LIGHTING );
    glDisable( GL_BLEND );
    glEnable( GL_TEXTURE_2D );

    wasUnderWater = isUnderWater;
    isUnderWater  = camera.p.z < 0.0f;

    terra.setRadius( frustum.radius );
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
        waterStructures << str;
      }
    }
    structures.clear();

    BSP::endRender();

    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );

    // draw (non-delayed) objects
    objects.sort();

    for( int i = 0; i < objects.length(); ++i ) {
      const Object* obj = objects[i].obj;

      if( obj->index == camera.tagged ) {
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colors::TAG );
      }

      glPushMatrix();
      glTranslatef( obj->p.x, obj->p.y, obj->p.z );

      drawModel( obj, null );

      glPopMatrix();

      if( obj->index == camera.tagged ) {
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colors::BLACK );
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
    particles.clear();

    assert( glGetError() == GL_NO_ERROR );

    glColor4fv( Colors::WHITE );
    glEnable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );

    // draw delayed objects
    glDisable( GL_COLOR_MATERIAL );

    for( int i = 0; i < delayedObjects.length(); ++i ) {
      const Object* obj = delayedObjects[i].obj;

      if( obj->index == camera.tagged ) {
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colors::TAG );
      }

      glPushMatrix();
      glTranslatef( obj->p.x, obj->p.y, obj->p.z );

      drawModel( obj, null );

      glPopMatrix();

      if( obj->index == camera.tagged ) {
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colors::BLACK );
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
    waterStructures.clear();

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
            Colors::CLIP_AABB : Colors::NOCLIP_AABB );
        shape.drawBox( *objects[i].obj );
      }

      glColor4fv( Colors::WHITE );
      glDisable( GL_COLOR_MATERIAL );
    }

    objects.clear();
    delayedObjects.clear();

    glDisable( GL_FOG );

    glDisable( GL_DEPTH_TEST );
    glColor4fv( Colors::WHITE );

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
      class tm t;

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
      for( typeof( models.begin() ) i = models.begin(); !i.isPast(); ) {
        Model* model = *i;
        uint   key   = i.key();

        // we should advance now, so that we don't remove the element the iterator is pointing at
        ++i;

        if( model->flags & Model::UPDATED_BIT ) {
          model->flags &= ~Model::UPDATED_BIT;
        }
        else {
          models.remove( key );
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
    for( typeof( models.begin() ) i = models.begin(); !i.isPast(); ) {
      Model* model = i.value();
      uint   key   = i.key();
      ++i;

      if( world.objects[key] == null ) {
        delete model;
        models.remove( key );
      }
    }
  }

  void Render::update()
  {
    drawWorld();
    drawCommon();
  }

  void Render::init()
  {
    doScreenshot = false;
    clearCount   = 0;

    log.println( "Initializing Graphics {" );
    log.indent();

    String sExtensions = reinterpret_cast<const char*>( glGetString( GL_EXTENSIONS ) );
    Vector<String> extensions = sExtensions.trim().split( ' ' );

    log.println( "OpenGL vendor: %s", glGetString( GL_VENDOR ) );
    log.println( "OpenGL renderer: %s", glGetString( GL_RENDERER ) );
    log.println( "OpenGL version: %s", glGetString( GL_VERSION ) );
    log.println( "OpenGL extensions {" );
    log.indent();
    foreach( extension, extensions.begin() ) {
      log.println( "%s", extension->cstr() );
    }

#ifdef OZ_MINGW32
    if( glActiveTexture == null ) {
      glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>( SDL_GL_GetProcAddress( "glActiveTexture" ) );
    }
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
      bsps << null;
    }

    glEnable( GL_POINT_SMOOTH );
    glPointSize( float( camera.height ) * STAR_SIZE );

    // fog
    glFogi( GL_FOG_MODE, GL_LINEAR );
    glFogf( GL_FOG_START, 0.0f );

    // lighting
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
    glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, Colors::GLOBAL_AMBIENT );
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::WHITE );
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
    models.deallocate();

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
