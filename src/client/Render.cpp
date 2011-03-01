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

#include "matrix/Collider.hpp"
#include "matrix/Physics.hpp"
#include "matrix/BotClass.hpp"

#include "client/Frustum.hpp"
#include "client/Colours.hpp"
#include "client/Shape.hpp"

#include "client/Water.hpp"
#include "client/Sky.hpp"
#include "client/Terra.hpp"
#include "client/BSP.hpp"

#include "client/SMMModel.hpp"
#include "client/SMMVehicleModel.hpp"
#include "client/ExplosionModel.hpp"
#include "client/MD2Model.hpp"
#include "client/MD2WeaponModel.hpp"

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

  void Render::scheduleCell( int cellX, int cellY )
  {
    Cell& cell = orbis.cells[cellX][cellY];

    for( int i = 0; i < cell.structs.length(); ++i ) {
      Struct* str = orbis.structs[ cell.structs[i] ];

      if( !drawnStructs.get( cell.structs[i] ) && frustum.isVisible( *str ) ) {
        drawnStructs.set( cell.structs[i] );
        structs.add( str );
      }
    }

    foreach( obj, cell.objects.citer() ) {
      if( obj->flags & Object::NO_DRAW_BIT ) {
        continue;
      }
      bool isVisible =
          ( obj->flags & Object::WIDE_CULL_BIT ) ?
              frustum.isVisible( *obj, WIDE_CULL_FACTOR ) :
              frustum.isVisible( *obj );

      if( isVisible ) {
        ObjectEntry entry( ( obj->p - camera.p ).sqL(), obj );

        objects.add( entry );
      }
    }

    foreach( part, cell.particles.citer() ) {
      if( frustum.isVisible( part->p, particleRadius ) ) {
        particles.add( part );
      }
    }

  }

  void Render::drawOrbis()
  {
    hard_assert( glGetError() == GL_NO_ERROR );

    collider.translate( camera.p, Vec3::ZERO );
    isUnderWater = collider.hit.inWater;

    // clear colour, visibility, fog
    if( isUnderWater ) {
      visibility = waterNightVisibility + sky.ratio * ( waterDayVisibility - waterNightVisibility );

      glClearColor( Colours::water[0], Colours::water[1], Colours::water[2], Colours::water[3] );
      glFogfv( GL_FOG_COLOR, Colours::water );
      glFogf( GL_FOG_END, visibility );
    }
    else {
      visibility = nightVisibility + sky.ratio * ( dayVisibility - nightVisibility );

      glClearColor( Colours::sky[0], Colours::sky[1], Colours::sky[2], Colours::sky[3] );
      glFogfv( GL_FOG_COLOR, Colours::sky );
      glFogf( GL_FOG_END, visibility );
    }

    // frustum
    Span span;
    frustum.update();
    frustum.getExtrems( span, camera.p );

    span.minX = max( span.minX - 2, 0 );
    span.maxX = min( span.maxX + 2, Orbis::MAX - 1 );
    span.minY = max( span.minY - 2, 0 );
    span.maxY = min( span.maxY + 2, Orbis::MAX - 1 );

    sky.update();
    water.update();

    // drawnStructs
    if( drawnStructs.length() < orbis.structs.length() ) {
      drawnStructs.setSize( orbis.structs.length() );
    }
    drawnStructs.clearAll();

    float minXCentre = float( span.minX - Orbis::MAX / 2 ) * Cell::SIZE + Cell::SIZE / 2.0f;
    float minYCentre = float( span.minY - Orbis::MAX / 2 ) * Cell::SIZE + Cell::SIZE / 2.0f;

    float x = minXCentre;
    for( int i = span.minX; i <= span.maxX; ++i, x += Cell::SIZE ) {
      float y = minYCentre;

      for( int j = span.minY; j <= span.maxY; ++j, y += Cell::SIZE ) {
        if( frustum.isVisible( x, y, CELL_WIDE_RADIUS  ) ) {
          scheduleCell( i, j );
        }
      }
    }

    // clear buffer
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    hard_assert( glIsEnabled( GL_TEXTURE_2D ) );
    hard_assert( !glIsEnabled( GL_BLEND ) );

    // camera transformation
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -camera.vertPlane, +camera.vertPlane, -camera.horizPlane, +camera.horizPlane,
               camera.minDist, camera.maxDist );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glMultMatrixf( camera.rotTMat );

    if( !isUnderWater ) {
      sky.draw();
    }

    glTranslatef( -camera.p.x, -camera.p.y, -camera.p.z );

    shader.use( Shader::DEFAULT );

    // lighting
    glLightfv( GL_LIGHT0, GL_POSITION, sky.lightDir );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, Colours::diffuse );
    glLightfv( GL_LIGHT0, GL_AMBIENT, Colours::ambient );

//     glUniform3fv( Param::oz_AmbientLight, 1, Colours::GLOBAL_AMBIENT + Colours::ambient );
//     glUniform3fv( Param::oz_SkyLight, 2, Shader::Light( sky.lightDir, Colours::diffuse ) );
//     glUniform3fv( Param::oz_PointLights, 1,
//                   Shader::Light( Point3( 52, -44, 37 ), Quat( 1.0f, 1.0f, 1.0f, 1.0f ) ) );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_FOG );
    glEnable( GL_LIGHTING );

    shader.bindTextures( 0 );

    hard_assert( !glIsEnabled( GL_BLEND ) );

    terra.draw();

    // draw structures
    foreach( str, structs.citer() ) {
      context.drawBSP( *str, Mesh::SOLID_BIT );
    }

    // draw objects
    objects.sort();

    for( int i = 0; i < objects.length(); ++i ) {
      const Object* obj = objects[i].obj;

      if( obj->index == camera.tagged ) {
        glUniform4fv( Param::oz_DiffuseMaterial, 1, Colours::TAG );
      }

      glPushMatrix();
      glTranslatef( obj->p.x, obj->p.y, obj->p.z );

      context.drawModel( obj, null );

      glPopMatrix();

      if( obj->index == camera.tagged ) {
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Colours::BLACK );
      }
    }

    hard_assert( !glIsEnabled( GL_BLEND ) );

    // draw particles
    glEnable( GL_BLEND );

    shader.bindTextures( 0 );
    shape.bindVertexArray();

    for( int i = 0; i < particles.length(); ++i ) {
      const Particle* part = particles[i];

      glPushMatrix();
      glTranslatef( part->p.x, part->p.y, part->p.z );

      shape.draw( part );

      glPopMatrix();
    }

    hard_assert( glGetError() == GL_NO_ERROR );

    glColor4fv( Colours::BLACK );

    // draw structures' water
    foreach( str, structs.citer() ) {
      context.drawBSP( *str, Mesh::ALPHA_BIT );
    }

    terra.drawWater();

    glDisable( GL_LIGHTING );
    glDisable( GL_BLEND );

    glUniform4f( Param::oz_DiffuseMaterial, 1.0f, 1.0f, 1.0f, 1.0f );

//     glUniform3fv( Param::oz_AmbientLight, 1, Colours::WHITE );
//     glUniform3fv( Param::oz_SkyLight, 2, Shader::Light::NONE );
//     glUniform3fv( Param::oz_PointLights, 16, Shader::Light::NONE );

    shader.bindTextures( 0 );
    shape.bindVertexArray();

    if( showAim ) {
      Vec3 move = camera.at * 32.0f;
      collider.translate( camera.p, move, camera.botObj );
      move *= collider.hit.ratio;

      glColor3f( 0.0f, 1.0f, 0.0f );
      shape.drawBox( AABB( camera.p + move, Vec3( 0.05f, 0.05f, 0.05f ) ) );
    }

    if( showBounds ) {
      for( int i = 0; i < objects.length(); ++i ) {
        glColor4fv( ( objects[i].obj->flags & Object::SOLID_BIT ) ?
            Colours::CLIP_AABB : Colours::NOCLIP_AABB );
        shape.drawWireBox( *objects[i].obj );
      }

      glColor4fv( Colours::STRUCTURE_AABB );

      for( int i = 0; i < structs.length(); ++i ) {
        const Struct* str = structs[i];

        glColor4fv( Colours::ENTITY_AABB );

        foreach( entity, citer( str->entities, str->nEntities ) ) {
          Bounds bb = str->toAbsoluteCS( *entity->model + entity->offset );
          shape.drawWireBox( bb.toAABB() );
        }

        glColor4fv( Colours::STRUCTURE_AABB );
        shape.drawWireBox( str->toAABB() );
      }
    }

    structs.clear();
    waterStructs.clear();
    objects.clear();
    particles.clear();

    glDisable( GL_FOG );

    glDisable( GL_DEPTH_TEST );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Render::drawCommon()
  {
    ui::ui.draw();

    SDL_GL_SwapBuffers();
  }

  void Render::draw()
  {
    drawOrbis();
    drawCommon();
  }

  void Render::load()
  {
    log.println( "Loading Render {" );
    log.indent();

    hard_assert( glGetError() == GL_NO_ERROR );

    shader.bindTextures( 0 );

    ui::ui.load();

    frustum.init();
    water.init();
    shape.load();
    sky.load();
    terra.load();

    structs.alloc( 64 );
    objects.alloc( 8192 );
    particles.alloc( 1024 );

    shader.load();
    shader.use( Shader::DEFAULT );

    // fog
    glFogi( GL_FOG_MODE, GL_LINEAR );
    glFogf( GL_FOG_START, 0.0f );

    // lighting
//     glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, Colours::GLOBAL_AMBIENT );
    glUniform4fv( Param::oz_DiffuseMaterial, 1, Colours::WHITE );
    glUniform4fv( Param::oz_SpecularMaterial, 1, Colours::BLACK );
    glEnable( GL_LIGHT0 );

    glUniform1f( Param::oz_TextureScale, 1.0f );
    glUniform4f( Param::oz_DiffuseMaterial, 1.0f, 1.0f, 1.0f, 1.0f );

//     glUniform3fv( Param::oz_AmbientLight, 1, Colours::WHITE );
//     glUniform3fv( Param::oz_SkyLight, 2, Shader::Light::NONE );
//     glUniform3fv( Param::oz_PointLights, 16, Shader::Light::NONE );

    log.unindent();
    log.println( "}" );
  }

  void Render::unload()
  {
    log.println( "Unloading Render {" );
    log.indent();

    shader.unload();

    drawnStructs.clear();

    terra.unload();
    sky.unload();
    shape.unload();

    structs.clear();
    structs.dealloc();

    objects.clear();
    objects.dealloc();

    particles.clear();
    particles.dealloc();

    waterStructs.clear();
    waterStructs.dealloc();

    ui::ui.unload();

    log.unindent();
    log.println( "}" );
  }

  void Render::init()
  {
    log.println( "Initialising Render {" );
    log.indent();

    SDL_GL_SwapBuffers();

    bool isVaoSupported = false;

    String version = String::cstr( glGetString( GL_VERSION ) );
    DArray<String> extensions;
    String sExtensions = String::cstr( glGetString( GL_EXTENSIONS ) );
    sExtensions.trim().split( ' ', &extensions );

    log.println( "OpenGL vendor: %s", glGetString( GL_VENDOR ) );
    log.println( "OpenGL renderer: %s", glGetString( GL_RENDERER ) );
    log.println( "OpenGL version: %s", version.cstr() );
    log.println( "OpenGL extensions {" );
    log.indent();

    foreach( extension, extensions.citer() ) {
      log.println( "%s", extension->cstr() );

      if( extension->equals( "GL_ARB_vertex_array_object" ) ) {
        isVaoSupported = true;
      }
    }

    log.unindent();
    log.println( "}" );

    int major = atoi( version );
    int minor = atoi( version + version.index( '.' ) + 1 );

    if( major < 2 || ( major == 2 && minor < 1 ) ) {
      log.println( "Error: at least OpenGL 2.1 required" );
      throw Exception( "Too old OpenGL version" );
    }

    if( !isVaoSupported ) {
      log.println( "Error: vertex array object (GL_ARB_vertex_array_object) is not supported" );
      throw Exception( "VAO not supported by OpenGL" );
    }

    dayVisibility        = config.getSet( "render.dayVisibility",        300.0f );
    nightVisibility      = config.getSet( "render.nightVisibility",      100.0f );
    waterDayVisibility   = config.getSet( "render.waterDayVisibility",   8.0f );
    waterNightVisibility = config.getSet( "render.waterNightVisibility", 4.0f );
    particleRadius       = config.getSet( "render.particleRadius",       0.5f );
    showBounds           = config.getSet( "render.showBounds",           false );
    showAim              = config.getSet( "render.showAim",              false );

    glEnable( GL_CULL_FACE );
    glDepthFunc( GL_LEQUAL );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );

    shader.init();
    camera.init();
    ui::ui.init();
    ui::ui.draw();

    hard_assert( glGetError() == GL_NO_ERROR );

    log.unindent();
    log.println( "}" );
  }

  void Render::free()
  {
    log.println( "Shutting down Render {" );
    log.indent();

    ui::ui.free();
    shader.free();

    log.unindent();
    log.println( "}" );
  }

}
}
