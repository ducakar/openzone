/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Render.cc
 */

#include "stable.hh"

#include "client/Render.hh"

#include "client/Frustum.hh"
#include "client/Colours.hh"
#include "client/Shape.hh"

#include "client/Caelum.hh"
#include "client/Terra.hh"

#include "client/SMMImago.hh"
#include "client/SMMVehicleImago.hh"
#include "client/ExplosionImago.hh"
#include "client/MD2WeaponImago.hh"

#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

Render render;

const float Render::WIDE_CULL_FACTOR = 6.0f;
const float Render::CELL_WIDE_RADIUS =
  ( float( Cell::SIZEI / 2 ) + float( Object::MAX_DIMI ) * WIDE_CULL_FACTOR ) * Math::SQRT_2;

const float Render::NIGHT_FOG_COEFF  = 2.0f;
const float Render::NIGHT_FOG_DIST   = 0.3f;
const float Render::WATER_VISIBILITY = 32.0f;
const float Render::LAVA_VISIBILITY  = 4.0f;

const float Render::WIND_FACTOR      = 0.0008f;
const float Render::WIND_PHI_INC     = 0.04f;

void Render::scheduleCell( int cellX, int cellY )
{
  const Cell& cell = orbis.cells[cellX][cellY];

  for( int i = 0; i < cell.structs.length(); ++i ) {
    if( !drawnStructs.get( cell.structs[i] ) ) {
      drawnStructs.set( cell.structs[i] );

      Struct* str    = orbis.structs[ cell.structs[i] ];
      Vec3    dim    = str->maxs - str->mins;
      Point3  p      = str->mins + 0.5f * dim;
      float   radius = dim.fastL();

      if( frustum.isVisible( p, radius ) ) {
        structs.add( ModelEntry( ( p - camera.p ).sqL(), str ) );
      }
    }
  }

  foreach( obj, cell.objects.citer() ) {
    float radius = obj->flags & Object::WIDE_CULL_BIT ?
        WIDE_CULL_FACTOR * obj->dim.fastL() : obj->dim.fastL();

    if( frustum.isVisible( obj->p, radius ) ) {
      objects.add( ModelEntry( ( obj->p - camera.p ).sqL(), obj ) );
    }
  }

  foreach( frag, cell.frags.citer() ) {
    if( frustum.isVisible( frag->p, FragPool::FRAG_RADIUS ) ) {
      frags.add( ModelEntry( ( frag->p - camera.p ).sqL(), frag ) );
    }
  }
}

void Render::prepareDraw()
{
  uint currentMicros = Time::uclock();
  uint beginMicros = currentMicros;

  if( camera.p.z < 0.0f ) {
    shader.medium = orbis.terra.liquid;
  }
  else {
    collider.translate( camera.p, Vec3::ZERO );
    shader.medium = collider.hit.medium;
  }

  visibility = shader.medium & Medium::WATER_BIT ? WATER_VISIBILITY :
               shader.medium & Medium::LAVA_BIT ? LAVA_VISIBILITY : visibilityRange;
  windPhi    = Math::fmod( windPhi + WIND_PHI_INC, Math::TAU );

  // frustum
  camera.maxDist = visibility;

  Span span;
  frustum.update();
  frustum.getExtrems( span, camera.p );

  caelum.update();

  // drawnStructs
  if( drawnStructs.length() < orbis.structs.length() ) {
    drawnStructs.dealloc();
    drawnStructs.alloc( orbis.structs.length() );
  }
  drawnStructs.clearAll();

  float minXCentre = float( span.minX - Orbis::CELLS / 2 ) * Cell::SIZE + Cell::SIZE / 2.0f;
  float minYCentre = float( span.minY - Orbis::CELLS / 2 ) * Cell::SIZE + Cell::SIZE / 2.0f;

  float x = minXCentre;
  for( int i = span.minX; i <= span.maxX; ++i, x += Cell::SIZE ) {
    float y = minYCentre;

    for( int j = span.minY; j <= span.maxY; ++j, y += Cell::SIZE ) {
      if( frustum.isVisible( x, y, CELL_WIDE_RADIUS ) ) {
        scheduleCell( i, j );
      }
    }
  }

  structs.sort();
  objects.sort();
  frags.sort();

  currentMicros = Time::uclock();
  prepareMicros += currentMicros - beginMicros;
}

void Render::drawGeometry()
{
  uint currentMicros = Time::uclock();
  uint beginMicros = currentMicros;

  OZ_GL_CHECK_ERROR();

  Vec4 clearColour = Colours::caelum;

  if( shader.medium & Medium::WATER_BIT ) {
    if( camera.nightVision ) {
      if( camera.p.z >= 0.0f ) {
        clearColour.x = 0.0f;
        clearColour.y = Colours::WATER.x + Colours::WATER.y + Colours::WATER.z;
        clearColour.z = 0.0f;
      }
      else {
        clearColour = Colours::liquid;
      }
    }
    else {
      clearColour = camera.p.z >= 0.0f ? Colours::WATER : Colours::liquid;
    }
  }
  else if( shader.medium & Medium::LAVA_BIT ) {
    if( camera.nightVision ) {
      clearColour.x = 0.0f;
      clearColour.y = Colours::LAVA.x + Colours::LAVA.y + Colours::LAVA.z;
      clearColour.z = 0.0f;
    }
    else {
      clearColour = Colours::LAVA;
    }
  }

  // clear buffer
  glClearColor( clearColour.x, clearColour.y, clearColour.z, clearColour.w );
  glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

  currentMicros = Time::uclock();
  swapMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  tf.camera = camera.rotTMat;

  if( !( shader.medium & Medium::LIQUID_MASK ) ) {
    tf.projection();

    caelum.draw();
  }

  currentMicros = Time::uclock();
  caelumMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  // camera transformation
  tf.projection();
  tf.camera.translate( Point3::ORIGIN - camera.p );

  shader.setAmbientLight( Colours::GLOBAL_AMBIENT + Colours::ambient );
  shader.setCaelumLight( caelum.lightDir, Colours::diffuse );

  // set shaders
  for( int i = 0; i < library.shaders.length(); ++i ) {
    shader.use( i );

    tf.applyCamera();
    shader.updateLights();

    glUniform1f( param.oz_Fog_start, shader.medium & Medium::LIQUID_MASK ? 0.0f : 50.0f );
    glUniform1f( param.oz_Fog_end, visibility );
    glUniform4fv( param.oz_Fog_colour, 1, clearColour );

    glUniform1i( param.oz_NightVision, camera.nightVision );

    glUniform4f( param.oz_Wind, 1.0f, 1.0f, WIND_FACTOR, windPhi );
  }

  Mesh::reset();

  glEnable( GL_DEPTH_TEST );

  currentMicros = Time::uclock();
  setupMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  // draw structures
  shader.use( shader.mesh );

  for( int i = 0; i < structs.length(); ++i ) {
    context.drawBSP( structs[i].str, Mesh::SOLID_BIT );
  }

  currentMicros = Time::uclock();
  structsMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  terra.draw();

  currentMicros = Time::uclock();
  terraMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  // draw objects
  for( int i = 0; i < objects.length(); ++i ) {
    context.drawImago( objects[i].obj, null, Mesh::SOLID_BIT );
  }

  currentMicros = Time::uclock();
  objectsMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  // draw fragments
  glEnable( GL_BLEND );
  shader.use( shader.mesh );

  glBindTexture( GL_TEXTURE_2D, 0 );
  glUniform1f( param.oz_Specular, 1.0f );

  for( int i = frags.length() - 1; i >= 0; --i ) {
    context.drawFrag( frags[i].frag );
  }

  // draw transparent parts of objects
  shader.colour = Colours::WHITE;

  currentMicros = Time::uclock();
  fragsMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  for( int i = objects.length() - 1; i >= 0; --i ) {
    context.drawImago( objects[i].obj, null, Mesh::ALPHA_BIT );
  }

  OZ_GL_CHECK_ERROR();

  currentMicros = Time::uclock();
  objectsMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  terra.drawWater();

  currentMicros = Time::uclock();
  terraMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  // draw structures' alpha parts
  shader.use( shader.mesh );

  for( int i = structs.length() - 1; i >= 0; --i ) {
    context.drawBSP( structs[i].str, Mesh::ALPHA_BIT );
  }

  glDisable( GL_BLEND );

  currentMicros = Time::uclock();
  structsMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  shader.use( shader.plain );

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, 0 );

  shape.bindVertexArray();

  if( showAim ) {
    Vec3 move = camera.at * 32.0f;
    collider.translate( camera.p, move, camera.botObj );
    move *= collider.hit.ratio;

    glUniform4f( param.oz_Colour, 0.0f, 1.0f, 0.0f, 1.0f );
    shape.box( AABB( camera.p + move, Vec3( 0.05f, 0.05f, 0.05f ) ) );
  }

  if( showBounds ) {
    glLineWidth( 1.0f );

    for( int i = 0; i < objects.length(); ++i ) {
      glUniform4fv( param.oz_Colour, 1,
                    objects[i].obj->flags & Object::SOLID_BIT ?
                      Colours::CLIP_AABB : Colours::NOCLIP_AABB );
      shape.wireBox( *objects[i].obj );
    }

    glUniform4fv( param.oz_Colour, 1, Colours::STRUCTURE_AABB );

    for( int i = 0; i < structs.length(); ++i ) {
      const Struct* str = structs[i].str;

      glUniform4fv( param.oz_Colour, 1, Colours::ENTITY_AABB );

      foreach( entity, citer( str->entities, str->nEntities ) ) {
        Bounds bb = str->toAbsoluteCS( *entity->model + entity->offset );
        shape.wireBox( bb.toAABB() );
      }

      glUniform4fv( param.oz_Colour, 1, Colours::STRUCTURE_AABB );
      shape.wireBox( str->toAABB() );
    }
  }

  glDisable( GL_DEPTH_TEST );

  OZ_GL_CHECK_ERROR();

  structs.clear();
  waterStructs.clear();
  objects.clear();
  frags.clear();

  currentMicros = Time::uclock();
  miscMicros += currentMicros - beginMicros;
}

void Render::drawOrbis()
{
  if( isOffscreen ) {
    glPushAttrib( GL_VIEWPORT_BIT );
    glViewport( 0, 0, renderWidth, renderHeight );

    uint dbos[] = { GL_COLOR_ATTACHMENT0 };

    glBindFramebuffer( GL_FRAMEBUFFER, mainFrame );
    glDrawBuffers( 1, dbos );
  }

  prepareDraw();
  drawGeometry();

  uint beginMicros = Time::uclock();

  if( isOffscreen ) {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glPopAttrib();

    if( doPostprocess ) {
      tf.ortho( camera.width, camera.height );
      tf.camera = Mat44::ID;

      shader.use( shader.postprocess );
      tf.applyCamera();

      glBindTexture( GL_TEXTURE_2D, colourBuffer );
      shape.fill( 0, 0, camera.width, camera.height );
      glBindTexture( GL_TEXTURE_2D, 0 );
    }
    else {
      glBindFramebuffer( GL_READ_FRAMEBUFFER, mainFrame );

      glBlitFramebuffer( 0, 0, renderWidth, renderHeight,
                         0, 0, camera.width, camera.height,
                         GL_COLOR_BUFFER_BIT, offscreenFilter );

      glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
    }
  }

  OZ_GL_CHECK_ERROR();

  postprocessMicros += Time::uclock() - beginMicros;
}

void Render::drawUI()
{
  uint beginMicros = Time::uclock();

  ui::ui.draw();

  uiMicros += Time::uclock() - beginMicros;
}

void Render::draw( int flags )
{
  if( flags & DRAW_ORBIS_BIT ) {
    shader.mode = Shader::SCENE;

    drawOrbis();
  }
  if( flags & DRAW_UI_BIT ) {
    shader.mode = Shader::UI;

    drawUI();
  }
}

void Render::swap()
{
  uint beginMicros = Time::uclock();

  SDL_GL_SwapBuffers();

  swapMicros += Time::uclock() - beginMicros;
}

void Render::load()
{
  log.print( "Loading Render ..." );

  OZ_GL_CHECK_ERROR();

  ui::ui.load();

  structs.alloc( 64 );
  objects.alloc( 8192 );
  frags.alloc( 1024 );

  prepareMicros     = 0;
  setupMicros       = 0;
  caelumMicros      = 0;
  terraMicros       = 0;
  structsMicros     = 0;
  objectsMicros     = 0;
  fragsMicros       = 0;
  miscMicros        = 0;
  postprocessMicros = 0;
  uiMicros          = 0;
  swapMicros        = 0;

  log.printEnd( " OK" );
}

void Render::unload()
{
  glFinish();

  log.print( "Unloading Render ..." );

  drawnStructs.dealloc();

  caelum.unload();
  terra.unload();

  structs.clear();
  structs.dealloc();

  objects.clear();
  objects.dealloc();

  frags.clear();
  frags.dealloc();

  waterStructs.clear();
  waterStructs.dealloc();

  ui::ui.unload();

  log.printEnd( " OK" );
}

void Render::init( SDL_Surface* window_, int windowWidth, int windowHeight, bool isBuild )
{
  log.println( "Initialising Render {" );
  log.indent();

  window = window_;

  bool isCatalyst  = false;
  bool hasVAO      = false;
  bool hasFBO      = false;
  bool hasFloatTex = false;
  bool hasS3TC     = false;

  String vendor      = String::cstr( glGetString( GL_VENDOR ) );
  String renderer    = String::cstr( glGetString( GL_RENDERER ) );
  String version     = String::cstr( glGetString( GL_VERSION ) );
  String glslVersion = String::cstr( glGetString( GL_SHADING_LANGUAGE_VERSION ) );
  String sExtensions = String::cstr( glGetString( GL_EXTENSIONS ) );
  DArray<String> extensions = sExtensions.trim().split( ' ' );

  log.println( "OpenGL vendor: %s", vendor.cstr() );
  log.println( "OpenGL renderer: %s", renderer.cstr() );
  log.println( "OpenGL version: %s", version.cstr() );
  log.println( "GLSL version: %s", glslVersion.cstr() );

  log.verboseMode = true;

  log.println( "OpenGL extensions {" );
  log.indent();

  if( strstr( vendor, "ATI" ) != null ) {
    isCatalyst = true;
  }
  foreach( extension, extensions.citer() ) {
    log.println( "%s", extension->cstr() );

    if( extension->equals( "GL_ARB_vertex_array_object" ) ) {
      hasVAO = true;
    }
    if( extension->equals( "GL_ARB_framebuffer_object" ) ) {
      hasFBO = true;
    }
    if( extension->equals( "GL_ARB_texture_float" ) ) {
      hasFloatTex = true;
    }
    if( extension->equals( "GL_EXT_texture_compression_s3tc" ) ) {
      hasS3TC = true;
    }
  }

  log.unindent();
  log.println( "}" );

  log.verboseMode = false;

  int major = atoi( version );
  int minor = atoi( version.cstr() + version.index( '.' ) + 1 );

  if( major < 2 || ( major == 2 && minor < 1 ) ) {
    throw Exception( "Too old OpenGL version, at least 2.1 required" );
  }

  if( isCatalyst ) {
    config.include( "shader.vertexTexture", "false" );
    config.include( "shader.setSamplerIndices", "true" );
  }
  if( !hasVAO ) {
#ifndef OZ_GL_COMPATIBLE
    throw Exception( "GL_ARB_vertex_array_object not supported by OpenGL" );
#endif
  }
  if( !hasFBO ) {
    throw Exception( "GL_ARB_framebuffer_object not supported by OpenGL" );
  }
  if( !hasFloatTex ) {
    throw Exception( "GL_ARB_texture_float not supported by OpenGL" );
  }
  if( hasS3TC ) {
    shader.hasS3TC = true;
  }

  glInit();

  if( isBuild ) {
    config.get( "shader.setSamplerIndices", false );
    config.get( "shader.vertexTexture", false );

    log.unindent();
    log.println( "}" );

    OZ_GL_CHECK_ERROR();
    return;
  }

  isOffscreen     = config.getSet( "render.offscreen",            true );
  isDeferred      = config.getSet( "render.deferred",             false );
  doPostprocess   = config.getSet( "render.postprocess",          true );
  isLowDetail     = config.getSet( "render.lowDetail",            false );

  renderScale     = config.getSet( "render.scale",                1.0f );

  visibilityRange = config.getSet( "render.visibilityRange",      300.0f );
  showBounds      = config.getSet( "render.showBounds",           false );
  showAim         = config.getSet( "render.showAim",              false );

  windPhi         = 0.0f;

  String sOffscreenFilter = config.getSet( "render.offscreenFilter", "LINEAR" );

  if( sOffscreenFilter.equals( "NEAREST" ) ) {
    offscreenFilter = GL_NEAREST;
  }
  else if( sOffscreenFilter.equals( "LINEAR" ) ) {
    offscreenFilter = GL_LINEAR;
  }
  else {
    throw Exception( "Invalid render.offscreenFilter '%s'. Must be either LINEAR or NEAREST.",
                     sOffscreenFilter.cstr() );
  }

  if( isOffscreen ) {
    renderWidth  = int( float( windowWidth  ) * renderScale + 0.5f );
    renderHeight = int( float( windowHeight ) * renderScale + 0.5f );
  }
  else {
    isDeferred    = false;
    doPostprocess = false;
    renderScale   = 1.0f;
    renderWidth   = windowWidth;
    renderHeight  = windowHeight;
  }

  if( isOffscreen ) {
    glGenRenderbuffers( 1, &depthBuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, depthBuffer );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderWidth, renderHeight );
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );

    glGenTextures( 1, &colourBuffer );
    glBindTexture( GL_TEXTURE_2D, colourBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, int( offscreenFilter ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, int( offscreenFilter ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0,
                  GL_RGB, GL_UNSIGNED_BYTE, null );

    glBindTexture( GL_TEXTURE_2D, 0 );

    glGenFramebuffers( 1, &mainFrame );
    glBindFramebuffer( GL_FRAMEBUFFER, mainFrame );

    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0 );

    if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
      throw Exception( "Main framebuffer creation failed" );
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  }

  glEnable( GL_CULL_FACE );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  shader.init();
  shape.load();
  camera.init( windowWidth, windowHeight );
  ui::ui.init();

  shader.use( shader.plain );

  glBindTexture( GL_TEXTURE_2D, 0 );
  shape.bindVertexArray();

  OZ_GL_CHECK_ERROR();

  log.unindent();
  log.println( "}" );
}

void Render::free( bool isBuild )
{
  if( isBuild ) {
    return;
  }

  log.println( "Freeing Render {" );
  log.indent();

  if( isOffscreen ) {
    glDeleteFramebuffers( 1, &mainFrame );
    glDeleteTextures( 1, &colourBuffer );
    glDeleteRenderbuffers( 1, &depthBuffer );
  }

  ui::ui.free();
  shape.unload();
  shader.free();

  OZ_GL_CHECK_ERROR();

  log.unindent();
  log.println( "}" );
}

}
}
