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

#include "Sky.h"
#include "Water.h"
#include "Terrain.h"
#include "BSP.h"

#include "MD2StaticModel.h"

namespace oz
{
namespace client
{

  Render render;

  const float Render::RELEASED_CULL_FACTOR = 5.0f;
  const float Render::INCH = 0.0252f;

  const float Render::BLACK[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  const float Render::WHITE[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  const float Render::GLOBAL_AMBIENT[] = { 0.2f, 0.2f, 0.2f, 1.0f };

  const float Render::NIGHT_FOG_COEFF = 2.0f;
  const float Render::NIGHT_FOG_DIST = 0.3f;

  void Render::init()
  {
    String sExtensions = (const char*) glGetString( GL_EXTENSIONS );
    Vector<String> extensions = sExtensions.trim().split( ' ' );

    logFile.println( "OpenGL vendor: %s", glGetString( GL_VENDOR ) );
    logFile.println( "OpenGL version: %s", glGetString( GL_VERSION ) );
    logFile.println( "OpenGL extensions {" );
    logFile.indent();
    foreach( extension, extensions.iterator() ) {
      logFile.println( "%s", extension->cstr() );
    }
    logFile.unindent();
    logFile.println( "}" );

    font.init( "base/font.png", 2.0f );

    int screenX = config.get( "screen.width", 1024 );
    int screenY = config.get( "screen.height", 768 );

    perspectiveAngle  = config.get( "render.perspective.angle", 80.0f );
    perspectiveAspect = config.get( "render.perspective.aspect", 0.0f );
    perspectiveMin    = config.get( "render.perspective.min", 0.1f );
    perspectiveMax    = config.get( "render.perspective.max", 300.0f );

    if( perspectiveAspect == 0.0f ) {
      perspectiveAspect = (float) screenX / (float) screenY;
    }

    glViewport( 0, 0, screenX, screenY );
    glMatrixMode( GL_PROJECTION );
      glLoadIdentity();
      gluPerspective( perspectiveAngle, perspectiveAspect, perspectiveMin, perspectiveMax );
    glMatrixMode( GL_MODELVIEW );

    glLoadIdentity();
    glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );

    glEnable( GL_TEXTURE_2D );

    font.print( -10, 0, "LOADING ..." );

    SDL_GL_SwapBuffers();

    assert( glGetError() == GL_NO_ERROR );
  }

  void Render::load()
  {
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_CULL_FACE );

    assert( glGetError() == GL_NO_ERROR );

    // fog
    glFogi( GL_FOG_MODE, GL_LINEAR );
    glFogf( GL_FOG_START, 0.0f );
    glFogf( GL_FOG_END, perspectiveMax );

    assert( glGetError() == GL_NO_ERROR );

    // lighting
    glLightModeli(  GL_LIGHT_MODEL_TWO_SIDE, false );
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, GLOBAL_AMBIENT );

    assert( glGetError() == GL_NO_ERROR );

    glEnable( GL_COLOR_MATERIAL );
    glColor4fv( WHITE );
    glEnable( GL_LIGHT0 );

    assert( glGetError() == GL_NO_ERROR );

    particleRadius = config.get( "render.particleRadius", 0.5f );
    drawAABBs      = config.get( "render.drawAABBs",      false );
    showAim        = config.get( "render.showAim",        false );
    blendHeaven    = config.get( "render.blendHeaven",    false );

    camera.init();
    frustum.init( perspectiveAngle, perspectiveAspect, perspectiveMax );
    sky.init();
    terra.init();

    assert( glGetError() == GL_NO_ERROR );

    for( int i = 0; i < translator.bsps.length(); i++ ) {
      bsps << new BSP( world.bsps[i] );
    }

    // prepare for first frame
    glEnable( GL_DEPTH_TEST );

    glDisable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glEnable( GL_FOG );

    glEnable( GL_LIGHTING );

    assert( glGetError() == GL_NO_ERROR );
  }

  void Render::drawObject( Object *obj )
  {
    glPushMatrix();

    glTranslatef( obj->p.x, obj->p.y, obj->p.z );

    /*if( obj->flags & Object::WATER_BIT ) {
      waterObjects << obj;
    }
    else*/ if( obj->flags & Object::BLEND_BIT ) {
      blendedObjects << obj;
    }
    else {
      if( !models.contains( (uint) obj ) ) {
        models.add( (uint) obj, context.createModel( obj ) );
      }
      // draw model
      models.cachedValue()->draw();
      models.cachedValue()->state = Model::UPDATED;
    }
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
    glPopMatrix();
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
      if( &*obj == camera.bot ) {
        continue;
      }
      bool isVisible =
          ( obj->flags & Object::RELEASED_CULL_BIT ) ?
          frustum.isVisible( *obj * RELEASED_CULL_FACTOR ) :
          frustum.isVisible( *obj );

      if( isVisible ) {
        /*if( ( obj->flags & Object::WATER_BIT ) && obj->includes( camera.p ) ) {
          isUnderWater = true;

          waterObjects << obj;
        }
        else*/ if( obj->flags & Object::BLEND_BIT ) {
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

  void Render::draw()
  {
    assert( glGetError() == GL_NO_ERROR );

    // frustum
    frustum.update();
    frustum.getExtrems( camera.p );

    assert( glGetError() == GL_NO_ERROR );

    sky.update();
    water.update();

    assert( glGetError() == GL_NO_ERROR );

    bool wasUnderWater = isUnderWater;
    isUnderWater = false;

    // drawnStructures
    if( drawnStructures.length() != world.structures.length() ) {
      drawnStructures.setSize( world.structures.length() );
    }
    drawnStructures.clearAll();

    float minXCenter = (float) ( ( frustum.minX - World::MAX / 2 ) * Sector::DIM ) +
        Sector::DIM / 2.0f;
    float minYCenter = (float) ( ( frustum.minY - World::MAX / 2 ) * Sector::DIM ) +
        Sector::DIM / 2.0f;

    float x = minXCenter;
    for( int i = frustum.minX; i <= frustum.maxX; i++, x += Sector::DIM ) {
      float y = minYCenter;

      for( int j = frustum.minY; j <= frustum.maxY; j++, y += Sector::DIM ) {
        if( frustum.isVisible( x, y, Sector::RADIUS ) ) {
          scheduleSector( i, j );
        }
      }
    }

    // BEGIN RENDER
    if( isUnderWater ) {
      if( !wasUnderWater ) {
        glClearColor( water.COLOR[0], water.COLOR[1], water.COLOR[2], water.COLOR[3] );
        glFogfv( GL_FOG_COLOR, water.COLOR );
        glFogf( GL_FOG_END, Water::VISIBILITY );
      }
    }
    else {
      // we have to set this every time, since sky color changes all the time
      glClearColor( sky.color[0], sky.color[1], sky.color[2], sky.color[3] );
      glFogfv( GL_FOG_COLOR, sky.color );
      glFogf( GL_FOG_END,
              bound( NIGHT_FOG_COEFF * sky.lightDir[2], NIGHT_FOG_DIST, 1.0f ) * perspectiveMax );
    }
    // clear buffer
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    assert( glGetError() == GL_NO_ERROR );

    // camera transformation
    glMultMatrixf( camera.rotTMat );
    glTranslatef( -camera.p.x, -camera.p.y, -camera.p.z );

    assert( glGetError() == GL_NO_ERROR );

    // lighting
    glLightfv( GL_LIGHT0, GL_POSITION, sky.lightDir );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, sky.diffuseColor );
    glLightfv( GL_LIGHT0, GL_AMBIENT, sky.ambientColor );

    assert( glGetError() == GL_NO_ERROR );

    terra.draw();
    assert( glGetError() == GL_NO_ERROR );

    // draw structures
    BSP::beginRender();

    bool isInWaterBrush = false;
    for( int i = 0; i < structures.length(); i++ ) {
      Structure *str = structures[i];

      isInWaterBrush |= bsps[str->bsp]->draw( str );
      assert( glGetError() == GL_NO_ERROR );
    }
    structures.clear();

    BSP::endRender();

    // draw objects
    for( int i = 0; i < objects.length(); i++ ) {
      drawObject( objects[i] );
      assert( glGetError() == GL_NO_ERROR );
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

    // draw water
//     for( int i = 0; i < waterObjects.length(); i++ ) {
//       water.draw( waterObjects[i], isUnderWater );
//     }
//     waterObjects.clear();

    assert( glGetError() == GL_NO_ERROR );

    glColor4fv( WHITE );
    glDisable( GL_BLEND );

    if( showAim ) {
      Vec3 move = camera.at * 32.0f;
      collider.translate( camera.p, move, camera.bot );
      move *= collider.hit.ratio;

      glDisable( GL_TEXTURE_2D );
      glDisable( GL_LIGHTING );
      glColor3f( 0.0f, 1.0f, 0.0f );
      shape.drawBox( AABB( camera.p + move, Vec3( 0.03f, 0.03f, 0.03f ) ) );
      glColor3fv( WHITE );
      glEnable( GL_TEXTURE_2D );
    }

    assert( glGetError() == GL_NO_ERROR );

    glLoadIdentity();
    glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_FOG );
    glDisable( GL_LIGHTING );

    glEnable( GL_BLEND );
    glBlendFunc( GL_ONE, GL_ONE );

    assert( glGetError() == GL_NO_ERROR );

    if( camera.bot != null ) {
      font.print( -45, 37, "cam.p( %.2f %.2f %.2f ) bot( %.2f %.2f ) rel( %.2f %.2f )",
                  camera.p.x, camera.p.y, camera.p.z,
                  camera.bot->h, camera.bot->v, camera.h, camera.v );
      font.print( -45, 35, "camera.vel ( %.2f %.2f %.2f ) camera.mom ( %.2f %.2f %.2f )",
                  camera.bot->velocity.x, camera.bot->velocity.y, camera.bot->velocity.z,
                  camera.bot->momentum.x, camera.bot->momentum.y, camera.bot->momentum.z);

      font.print( -45, 33, "d %d fl %d lw %d h %d fr %d iw %d uw %d ld %d s %d ovlp %d wb %d",
                  ( camera.bot->flags & Object::DISABLED_BIT ) != 0,
                  ( camera.bot->flags & Object::ON_FLOOR_BIT ) != 0,
                  camera.bot->lower >= 0,
                  ( camera.bot->flags & Object::HIT_BIT ) != 0,
                  ( camera.bot->flags & Object::FRICTING_BIT ) != 0,
                  ( camera.bot->flags & Object::IN_WATER_BIT ) != 0,
                  ( camera.bot->flags & Object::UNDER_WATER_BIT ) != 0,
                  ( camera.bot->flags & Object::ON_LADDER_BIT ) != 0,
                  ( camera.bot->flags & Object::ON_SLICK_BIT ) != 0,
                  collider.test( *camera.bot ),
                  isInWaterBrush );
    }

    SDL_GL_SwapBuffers();

    assert( glGetError() == GL_NO_ERROR );

    // get ready for next frame
    glEnable( GL_DEPTH_TEST );

    glDisable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glEnable( GL_FOG );

    glEnable( GL_LIGHTING );

    assert( glGetError() == GL_NO_ERROR );

    // remove droped models
    foreach( model, models.iterator() ) {
      if( ( *model )->state == Model::NOT_UPDATED ) {
        models.remove( (uint) &*model );
      }
      else {
        ( *model )->state = Model::NOT_UPDATED;
      }
    }
  }

  void Render::free()
  {
    models.free();
    bsps.free();
  }

}
}
