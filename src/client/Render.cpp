/*
 *  Render.cpp
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Render.h"

#include "matrix/Matrix.h"
#include "matrix/Physics.h"

#include "Frustum.h"
#include "Shape.h"

#include "Water.h"
#include "Sky.h"
#include "Terrain.h"
#include "BSP.h"

#include "MD2StaticModel.h"

#include <time.h>

namespace oz
{
namespace client
{

  Render render;

  const float Render::RELEASED_CULL_FACTOR = 5.0f;

  const float Render::BLACK[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  const float Render::WHITE[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  const float Render::GLOBAL_AMBIENT[] = { 0.2f, 0.2f, 0.2f, 1.0f };

  const float Render::NIGHT_FOG_COEFF = 2.0f;
  const float Render::NIGHT_FOG_DIST = 0.3f;

  const float Render::WATER_COLOR[] = { 0.0f, 0.05f, 0.25f, 1.0f };
  const float Render::WATER_VISIBILITY = 8.0f;

  void Render::init()
  {
    doScreenshot = false;
    clearCount   = 0;

    log.println( "Initializing Graphics {" );
    log.indent();

    String sExtensions = (const char*) glGetString( GL_EXTENSIONS );
    Vector<String> extensions = sExtensions.trim().split( ' ' );

    log.println( "OpenGL vendor: %s", glGetString( GL_VENDOR ) );
    log.println( "OpenGL renderer: %s", glGetString( GL_RENDERER ) );
    log.println( "OpenGL version: %s", glGetString( GL_VERSION ) );
    log.println( "OpenGL extensions {" );
    log.indent();
    foreach( extension, extensions.iterator() ) {
      log.println( "%s", extension->cstr() );
    }
    log.unindent();
    log.println( "}" );

    screenX = config.get( "screen.width", 1024 );
    screenY = config.get( "screen.height", 768 );

    perspectiveAngle  = config.get( "render.perspective.angle", 80.0f );
    perspectiveAspect = config.get( "render.perspective.aspect", 0.0f );
    perspectiveMin    = config.get( "render.perspective.min", 0.1f );
    perspectiveMax    = config.get( "render.perspective.max", 300.0f );

    if( perspectiveAspect == 0.0 ) {
      perspectiveAspect = (double) screenX / (double) screenY;
    }

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, screenX, 0.0, screenY, 0.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glEnable( GL_TEXTURE_2D );
    glDepthFunc( GL_LEQUAL );

    ui::init( screenX, screenY );
    ui::root.add( new ui::DebugArea() );
    ui::root.add( new ui::HealthArea() );
    ui::root.add( new ui::BuildMenu(), -1, -1 );

    SDL_GL_SwapBuffers();

    assert( glGetError() == GL_NO_ERROR );

    log.unindent();
    log.println( "}" );
  }

  void Render::load()
  {
    log.println( "Loading Graphics {" );
    log.indent();

    assert( glGetError() == GL_NO_ERROR );

    particleRadius = config.get( "render.particleRadius", 0.5f );
    drawAABBs      = config.get( "render.drawAABBs",      false );
    showAim        = config.get( "render.showAim",        false );
    blendHeaven    = config.get( "render.blendHeaven",    false );

    camera.init();
    frustum.init( perspectiveAngle, perspectiveAspect, perspectiveMax );
    sky.init();
    terra.init();

    for( int i = 0; i < translator.bsps.length(); i++ ) {
      bsps << new BSP( world.bsps[i] );
    }

    // fog
    glFogi( GL_FOG_MODE, GL_LINEAR );
    glFogf( GL_FOG_START, 0.0f );

    // lighting
    glLightModeli(  GL_LIGHT_MODEL_TWO_SIDE, false );
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, GLOBAL_AMBIENT );

    glEnable( GL_COLOR_MATERIAL );
    glColor4fv( WHITE );
    glEnable( GL_LIGHT0 );

    glClearColor( sky.color[0], sky.color[1], sky.color[2], sky.color[3] );
    glFogfv( GL_FOG_COLOR, sky.color );
    glFogf( GL_FOG_END,
            bound( NIGHT_FOG_COEFF * sky.lightDir[2], NIGHT_FOG_DIST, 1.0f ) * perspectiveMax );

    log.unindent();
    log.println( "}" );
  }

  void Render::drawObject( Object *obj )
  {
    glPushMatrix();
    glTranslatef( obj->p.x, obj->p.y, obj->p.z );

    if( obj->flags & Object::BLEND_BIT ) {
      blendedObjects << obj;
    }
    else {
      if( !models.contains( obj->index ) ) {
        models.add( obj->index, context.createModel( obj ) );
      }
      // draw model
      models.cachedValue()->draw();
    }
    glPopMatrix();

    if( drawAABBs ) {
      glDisable( GL_LIGHTING );
      glDisable( GL_TEXTURE_2D );
      glEnable( GL_BLEND );
      glColor4f( 1.0f, 1.0f, 0.0f, 0.3f );

      shape.drawBox( *obj );

      glColor4fv( WHITE );
      glDisable( GL_BLEND );
      glEnable( GL_TEXTURE_2D );
      glEnable( GL_LIGHTING );
    }
  }

  void Render::scheduleSector( int sectorX, int sectorY )
  {
    Sector &sector = world.sectors[sectorX][sectorY];

    for( int i = 0; i < sector.structures.length(); i++ ) {
      Structure *str = world.structures[ sector.structures[i] ];

      if( !drawnStructures.get( sector.structures[i] ) && frustum.isVisible( *str ) ) {
        drawnStructures.set( sector.structures[i] );
        structures << str;
      }
    }

    foreach( obj, sector.objects.iterator() ) {
      if( !camera.isThirdPerson && &*obj == camera.bot ) {
        continue;
      }
      if( models.contains( obj->index ) ) {
        models.cachedValue()->isUpdated = true;
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

    foreach( part, sector.particles.iterator() ) {
      if( frustum.isVisible( part->p, particleRadius ) ) {
        particles << part;
      }
    }
  }

  void Render::sync()
  {
    foreach( i, synapse.cutObjects.iterator() ) {
      if( models.contains( ( *i )->index ) ) {
        delete models.cachedValue();
        models.remove( ( *i )->index );
      }
    }
    foreach( i, synapse.removeObjects.iterator() ) {
      if( models.contains( ( *i )->index ) ) {
        delete models.cachedValue();
        models.remove( ( *i )->index );
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

    float minXCenter = (float) ( ( frustum.minX - World::MAX / 2 ) * Sector::SIZE ) +
        Sector::SIZE / 2.0f;
    float minYCenter = (float) ( ( frustum.minY - World::MAX / 2 ) * Sector::SIZE ) +
        Sector::SIZE / 2.0f;

    float x = minXCenter;
    for( int i = frustum.minX; i <= frustum.maxX; i++, x += Sector::SIZE ) {
      float y = minYCenter;

      for( int j = frustum.minY; j <= frustum.maxY; j++, y += Sector::SIZE ) {
        if( frustum.isVisible( x, y, Sector::RADIUS ) ) {
          scheduleSector( i, j );
        }
      }
    }

    // BEGIN RENDER

    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_FOG );
    glEnable( GL_LIGHTING );
    glDisable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );

    // fog
    if( isUnderWater ) {
      if( !wasUnderWater ) {
        glClearColor( WATER_COLOR[0], WATER_COLOR[1], WATER_COLOR[2], WATER_COLOR[3] );
        glFogfv( GL_FOG_COLOR, WATER_COLOR );
        glFogf( GL_FOG_END, WATER_VISIBILITY );
      }
    }
    else {
      if( wasUnderWater ) {
        // we have to set this every time, since sky color changes all the time
        glClearColor( sky.color[0], sky.color[1], sky.color[2], sky.color[3] );
        glFogfv( GL_FOG_COLOR, sky.color );
        glFogf( GL_FOG_END,
                bound( NIGHT_FOG_COEFF * sky.lightDir[2], NIGHT_FOG_DIST, 1.0f ) * perspectiveMax );
      }
    }

    wasUnderWater = isUnderWater;
    isUnderWater  = camera.p.z < 0.0f;

    // clear buffer
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    // camera transformation
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( perspectiveAngle, perspectiveAspect, perspectiveMin, perspectiveMax );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );

    glMultMatrixf( camera.rotTMat );
    glTranslatef( -camera.p.x, -camera.p.y, -camera.p.z );

    // lighting
    glLightfv( GL_LIGHT0, GL_POSITION, sky.lightDir );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, sky.diffuseColor );
    glLightfv( GL_LIGHT0, GL_AMBIENT, sky.ambientColor );

    terra.setRadius( frustum.radius );
    terra.draw();

    // draw structures
    BSP::beginRender();

    for( int i = 0; i < structures.length(); i++ ) {
      Structure *str = structures[i];
      int waterFlags = bsps[str->bsp]->draw( str );

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
    objects.clear();

    // draw particles
    glDisable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );

    for( int i = 0; i < particles.length(); i++ ) {
      Particle *part = particles[i];

      glPushMatrix();
        glTranslatef( part->p.x, part->p.y, part->p.z );

        glRotatef( part->rot.y, 0.0f, 1.0f, 0.0f );
        glRotatef( part->rot.x, 1.0f, 0.0f, 0.0f );
        glRotatef( part->rot.z, 0.0f, 0.0f, 1.0f );

        glColor4f( part->color.x, part->color.y, part->color.z, part->lifeTime );
        // TODO particle render
        //glCallList( lists[part->model] );
      glPopMatrix();
    }
    particles.clear();

    glColor4fv( WHITE );
    glEnable( GL_TEXTURE_2D );

    // draw transparent objects
    for( int i = 0; i < blendedObjects.length(); i++ ) {
      // TODO blendedObjects[i] draw;
    }
    blendedObjects.clear();

    // draw structures' water
    glEnable( GL_BLEND );

    BSP::beginRender();

    for( int i = 0; i < waterStructures.length(); i++ ) {
      Structure *str = waterStructures[i];
      bsps[str->bsp]->drawWater( str );
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
    glColor4fv( WHITE );

    ui::draw();

    if( doScreenshot ) {
      uint *pixels = new uint[screenX * screenY * 4];
      char fileName[1024];
      time_t ct;
      struct tm t;

      ct = time( null );
      t = *localtime( &ct );

      snprintf( fileName, 1024, "%s/screenshot %04d-%02d-%02d %02d:%02d:%02d.bmp",
                config.get( "dir.home", "" ),
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
      for( typeof( models.iterator() ) i( models ); !i.isPassed(); ) {
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
    clearCount += timer.frameMillis;
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

}
}
