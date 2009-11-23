/*
 *  Render.cpp
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
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

#include "MD2StaticModel.h"

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

  const float Render::NIGHT_FOG_COEFF = 2.0f;
  const float Render::NIGHT_FOG_DIST = 0.3f;
  const float Render::WATER_VISIBILITY = 8.0f;

  const float Render::STAR_SIZE = 1.0f / 400.0f;

  void Render::scheduleCell( int cellX, int cellY )
  {
    Cell &cell = world.cells[cellX][cellY];

    for( int i = 0; i < cell.structures.length(); i++ ) {
      Structure *str = world.structures[ cell.structures[i] ];

      if( !drawnStructures.get( cell.structures[i] ) && frustum.isVisible( *str ) ) {
        drawnStructures.set( cell.structures[i] );
        structures << str;
      }
    }

    foreach( obj, cell.objects.iterator() ) {
      if( ( obj->flags & Object::NODRAW_BIT ) || ( &*obj == camera.bot && !camera.isExternal ) ) {
        continue;
      }
      bool isVisible =
          ( obj->flags & Object::WIDE_CULL_BIT ) ?
          frustum.isVisible( *obj * RELEASED_CULL_FACTOR ) :
          frustum.isVisible( *obj );

      if( isVisible ) {
        if( obj->flags & Object::BLEND_BIT ) {
          blendedObjects << obj;
        }
        else {
          objects << obj;
        }
      }
    }

    foreach( part, cell.particles.iterator() ) {
      if( frustum.isVisible( part->p, particleRadius ) ) {
        particles << part;
      }
    }
  }

  void Render::drawObject( Object *obj )
  {
    if( obj->index == taggedObjIndex ) {
      glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colors::TAG );
    }

    glPushMatrix();
    glTranslatef( obj->p.x, obj->p.y, obj->p.z );

    if( !models.contains( obj->index ) ) {
      models.add( obj->index, context.createModel( obj ) );
    }
    // draw model
    models.cachedValue()->draw();
    models.cachedValue()->isUpdated = true;

    glPopMatrix();

    if( obj->index == taggedObjIndex ) {
      glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colors::BLACK );
    }
  }

  void Render::sync()
  {
    for( typeof( models.iterator() ) i = models.iterator(); !i.isPassed(); ) {
      Model *model = i.value();
      uint  key    = i.key();
      ++i;

      if( world.objects[key] == null ) {
        delete model;
        models.remove( key );
      }
    }
  }

  void Render::update()
  {
    assert( glGetError() == GL_NO_ERROR );

    // frustum
    frustum.update();
    frustum.getExtrems( camera.p );

    sky.update();

    // drawnStructures
    if( drawnStructures.length() != world.structures.length() ) {
      drawnStructures.setSize( world.structures.length() );
    }
    drawnStructures.clearAll();

    // highlight the object the camera is looking at
    taggedObjIndex = -1;
    if( camera.bot == null ) {
      collider.translate( camera.p, camera.at * 2.0f );
      taggedObjIndex = collider.hit.obj == null ? -1 : collider.hit.obj->index;
    }
    else if( camera.bot->grabObjIndex != -1 ) {
      taggedObjIndex = camera.bot->grabObjIndex;
    }
    else {
      // { hsine, hcosine, vsine, vcosine, vcosine * hsine, vcosine * hcosine }
      float hvsc[6];

      Math::sincos( Math::rad( camera.bot->h ), &hvsc[0], &hvsc[1] );
      Math::sincos( Math::rad( camera.bot->v ), &hvsc[2], &hvsc[3] );

      hvsc[4] = hvsc[3] * hvsc[0];
      hvsc[5] = hvsc[3] * hvsc[1];

      Vec3 at = Vec3( -hvsc[4], hvsc[5], hvsc[2] );

      float distance = static_cast<BotClass*>( camera.bot->type )->grabDistance;
      collider.translate( camera.bot->p + camera.bot->camPos, at * distance, camera.bot );
      taggedObjIndex = collider.hit.obj == null ? -1 : collider.hit.obj->index;
    }

    float minXCenter = static_cast<float>( ( frustum.minX - World::MAX / 2 ) * Cell::SIZE ) +
        Cell::SIZE / 2.0f;
    float minYCenter = static_cast<float>( ( frustum.minY - World::MAX / 2 ) * Cell::SIZE ) +
        Cell::SIZE / 2.0f;

    float x = minXCenter;
    for( int i = frustum.minX; i <= frustum.maxX; i++, x += Cell::SIZE ) {
      float y = minYCenter;

      for( int j = frustum.minY; j <= frustum.maxY; j++, y += Cell::SIZE ) {
        if( frustum.isVisible( x, y, Cell::RADIUS ) ) {
          scheduleCell( i, j );
        }
      }
    }

    // BEGIN RENDER

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

    // clear buffer
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    // camera transformation
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( perspectiveAngle, perspectiveAspect, perspectiveMin, visibility );

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

    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );

    wasUnderWater = isUnderWater;
    isUnderWater  = camera.p.z < 0.0f;

    terra.setRadius( frustum.radius );
    terra.draw();

    // draw structures
    BSP::beginRender();

    for( int i = 0; i < structures.length(); i++ ) {
      Structure *str = structures[i];

      int waterFlags = bsps[str->bsp]->fullDraw( str );

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

    // draw objects
    for( int i = 0; i < objects.length(); i++ ) {
      drawObject( objects[i] );
    }

    // draw particles
    glEnable( GL_COLOR_MATERIAL );
    glDisable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );

    for( int i = 0; i < particles.length(); i++ ) {
      Particle *part = particles[i];

      glPushMatrix();
      glTranslatef( part->p.x, part->p.y, part->p.z );

      shape.draw( part );

      glPopMatrix();
    }
    particles.clear();

    assert( glGetError() == GL_NO_ERROR );

    glColor4fv( Colors::WHITE );
    glEnable( GL_TEXTURE_2D );

    // draw transparent objects
    glDisable( GL_COLOR_MATERIAL );

    for( int i = 0; i < blendedObjects.length(); i++ ) {
      drawObject( blendedObjects[i] );
    }

    glEnable( GL_COLOR_MATERIAL );

    if( drawAABBs ) {
      glDisable( GL_LIGHTING );
      glDisable( GL_TEXTURE_2D );
      glEnable( GL_BLEND );
      glColor4fv( Colors::AABB );

      for( int i = 0; i < objects.length(); i++ ) {
        shape.drawBox( *objects[i] );
      }
      for( int i = 0; i < blendedObjects.length(); i++ ) {
        shape.drawBox( *blendedObjects[i] );
      }

      glColor4fv( Colors::WHITE );
      glEnable( GL_LIGHTING );
      glEnable( GL_TEXTURE_2D );
    }

    glDisable( GL_COLOR_MATERIAL );

    objects.clear();
    blendedObjects.clear();

    // draw structures' water
    BSP::beginRender();

    for( int i = 0; i < waterStructures.length(); i++ ) {
      Structure *str = waterStructures[i];

      bsps[str->bsp]->fullDrawWater( str );
    }
    waterStructures.clear();

    BSP::endRender();

    terra.drawWater();

    water.update();

    glDisable( GL_FOG );
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );

    if( showAim ) {
      Vec3 move = camera.at * 32.0f;
      collider.translate( camera.p, move, camera.bot );
      move *= collider.hit.ratio;

      glColor3f( 0.0f, 1.0f, 0.0f );
      shape.drawBox( AABB( camera.p + move, Vec3( 0.05f, 0.05f, 0.05f ) ) );
    }

    glDisable( GL_DEPTH_TEST );
    glColor4fv( Colors::WHITE );

    ui::hud->taggedObjIndex = taggedObjIndex;
    ui::draw();

    if( doScreenshot ) {
      uint *pixels = new uint[screenX * screenY * 4];
      char fileName[1024];
      time_t ct;
      struct tm t;

      ct = time( null );
      t = *localtime( &ct );

      snprintf( fileName, 1024, "%s/screenshot %04d-%02d-%02d %02d:%02d:%02d.bmp",
                config.get( "dir.rc", "" ),
                1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec );
      fileName[1023] = '\0';

      glReadPixels( 0, 0, screenX, screenY, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
      SDL_Surface *surf = SDL_CreateRGBSurfaceFrom( pixels, screenX, screenY, 32, screenX * 4,
                                                    0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 );
      // flip image
      for( int i = 0; i < screenY / 2; i++ ) {
        for( int j = 0; j < screenX; j++ ) {
          swap( pixels[i * screenX + j], pixels[( screenY - i - 1 ) * screenX + j] );
        }
      }
      SDL_SaveBMP( surf, fileName );
      SDL_FreeSurface( surf );
      delete[] pixels;

      doScreenshot = false;
    }

    SDL_GL_SwapBuffers();

    assert( glGetError() == GL_NO_ERROR );

    // cleanups
    if( clearCount >= CLEAR_INTERVAL ) {
      // remove unused models
      for( typeof( models.iterator() ) i = models.iterator(); !i.isPassed(); ) {
        Model *model = *i;
        uint  key    = i.key();

        // we should advance now, so that we don't remove the element the iterator is pointing at
        ++i;

        if( model->isUpdated ) {
          model->isUpdated = false;
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
    foreach( extension, extensions.iterator() ) {
      log.println( "%s", extension->cstr() );
    }

#ifdef OZ_MINGW32
    if( glActiveTexture == null ) {
      glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>( SDL_GL_GetProcAddress( "glActiveTexture" ) );
    }
#endif

    log.unindent();
    log.println( "}" );

    screenX = config.getSet( "screen.width", 1024 );
    screenY = config.getSet( "screen.height", 768 );

    perspectiveAngle  = config.getSet( "render.perspective.angle", 80.0f );
    perspectiveAspect = config.getSet( "render.perspective.aspect", 0.0f );
    perspectiveMin    = config.getSet( "render.perspective.min", 0.1f );
    perspectiveMax    = config.getSet( "render.perspective.max", 400.0f );

    if( perspectiveAspect == 0.0 ) {
      perspectiveAspect = static_cast<double>( screenX ) / static_cast<double>( screenY );
    }

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, screenX, 0.0, screenY, 0.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glEnable( GL_TEXTURE_2D );
    glDepthFunc( GL_LEQUAL );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    ui::init( screenX, screenY );
    ui::hud->add( new ui::DebugArea() );
    ui::hud->add( new ui::BuildMenu(), -1, -1 );
    ui::hud->add( new ui::InventoryMenu(), 0, 0 );

    SDL_GL_SwapBuffers();

    assert( glGetError() == GL_NO_ERROR );

    log.unindent();
    log.println( "}" );
  }

  void Render::free()
  {
    log.println( "Shutting down Graphics {" );
    log.indent();

    ui::free();
    models.free();
    bsps.free();

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

    camera.init();
    frustum.init( perspectiveAngle, perspectiveAspect, perspectiveMax );
    water.init();
    shape.load();
    sky.load();
    terra.load();
    MD2::init();

    for( int i = 0; i < translator.bsps.length(); i++ ) {
      bsps << new BSP( world.bsps[i] );
    }

    glColor4fv( Colors::WHITE );
    glDisable( GL_TEXTURE_2D );

    glEnable( GL_POINT_SMOOTH );
    glPointSize( screenY * STAR_SIZE );

    // fog
    glFogi( GL_FOG_MODE, GL_LINEAR );
    glFogf( GL_FOG_START, 0.0f );

    // lighting
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
    glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, Colors::GLOBAL_AMBIENT );
    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

    glEnable( GL_LIGHT0 );

    log.unindent();
    log.println( "}" );
  }

  void Render::unload()
  {
    terra.unload();
    sky.unload();
    shape.unload();
  }

}
}
