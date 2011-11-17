/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Render.cpp
 */

#include "stable.hpp"

#include "client/Render.hpp"

#include "matrix/Collider.hpp"
#include "matrix/Physics.hpp"

#include "client/Frustum.hpp"
#include "client/Colours.hpp"
#include "client/Shape.hpp"
#include "client/FragPool.hpp"

#include "client/Caelum.hpp"
#include "client/Terra.hpp"
#include "client/BSP.hpp"

#include "client/SMMImago.hpp"
#include "client/SMMVehicleImago.hpp"
#include "client/ExplosionImago.hpp"
#include "client/MD2Imago.hpp"
#include "client/MD2WeaponImago.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

Render render;

const float Render::WIDE_CULL_FACTOR = 6.0f;
const float Render::CELL_WIDE_RADIUS =
    ( float( Cell::SIZEI / 2 ) + AABB::MAX_DIM * WIDE_CULL_FACTOR ) * Math::sqrt( 2.0f );

const float Render::NIGHT_FOG_COEFF  = 2.0f;
const float Render::NIGHT_FOG_DIST   = 0.3f;
const float Render::WATER_VISIBILITY = 32.0f;

const float Render::WIND_FACTOR      = 0.0008f;
const float Render::WIND_PHI_INC     = 0.04f;

void Render::scheduleCell( int cellX, int cellY )
{
  const Cell& cell = orbis.cells[cellX][cellY];

  for( int i = 0; i < cell.structs.length(); ++i ) {
    Struct* str    = orbis.structs[ cell.structs[i] ];
    Vec3    dim    = str->maxs - str->mins;
    Point3  p      = str->mins + 0.5f * dim;
    float   radius = dim.fastL();

    if( !drawnStructs.get( cell.structs[i] ) && frustum.isVisible( p, radius ) ) {
      drawnStructs.set( cell.structs[i] );
      structs.add( ObjectEntry( ( p - camera.p ).sqL(), str ) );
    }
  }

  for( const Object* obj = cell.objects.first(); obj != null; obj = obj->next[0] ) {
    float radius = obj->flags & Object::WIDE_CULL_BIT ?
        WIDE_CULL_FACTOR * obj->dim.fastL() : obj->dim.fastL();

    if( frustum.isVisible( obj->p, radius ) ) {
      objects.add( ObjectEntry( ( obj->p - camera.p ).sqL(), obj ) );
    }
  }

  for( const Frag* frag = cell.frags.first(); frag != null; frag = frag->next[0] ) {
    if( frustum.isVisible( frag->p, FragPool::FRAG_RADIUS ) ) {
      frags.add( frag );
    }
  }
}

void Render::prepareDraw()
{
  uint currentTime = SDL_GetTicks();
  uint beginTime = currentTime;

  collider.translate( camera.p, Vec3::ZERO );
  shader.isInWater = ( collider.hit.medium & Material::WATER_BIT ) != 0;

  visibility = shader.isInWater ? WATER_VISIBILITY : visibilityRange;
  windPhi    = Math::mod( windPhi + WIND_PHI_INC, Math::TAU );

  // frustum
  camera.maxDist = visibility;

  Span span;
  frustum.update();
  frustum.getExtrems( span, camera.p );

  span.minX = max( span.minX - 2, 0 );
  span.maxX = min( span.maxX + 2, Orbis::MAX - 1 );
  span.minY = max( span.minY - 2, 0 );
  span.maxY = min( span.maxY + 2, Orbis::MAX - 1 );

  caelum.update();

  // drawnStructs
  if( drawnStructs.length() < orbis.structs.length() ) {
    drawnStructs.dealloc();
    drawnStructs.alloc( orbis.structs.length() );
  }
  drawnStructs.clearAll();

  float minXCentre = float( span.minX - Orbis::MAX / 2 ) * Cell::SIZE + Cell::SIZE / 2.0f;
  float minYCentre = float( span.minY - Orbis::MAX / 2 ) * Cell::SIZE + Cell::SIZE / 2.0f;

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

  currentTime = SDL_GetTicks();
  prepareMillis += currentTime - beginTime;
}

void Render::drawGeometry()
{
  uint currentTime = SDL_GetTicks();
  uint beginTime = currentTime;

  OZ_GL_CHECK_ERROR();

  Vec4 clearColour = shader.isInWater ? Colours::water : Colours::caelum;

  // clear buffer
  glClearColor( clearColour.x, clearColour.y, clearColour.z, clearColour.w );
  glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

  tf.camera = camera.rotTMat;

  if( !shader.isInWater ) {
    camera.maxDist = 100.0f;
    tf.projection();

    caelum.draw();

    camera.maxDist = visibility;
  }

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

    glUniform1f( param.oz_Specular, 1.0f );

    glUniform1f( param.oz_Fog_start, shader.isInWater ? 0.0f : 100.0f );
    glUniform1f( param.oz_Fog_end, visibility );
    glUniform4fv( param.oz_Fog_colour, 1, clearColour );

    glUniform4f( param.oz_Wind, 1.0f, 1.0f, WIND_FACTOR, windPhi );
  }

  glEnable( GL_DEPTH_TEST );

  currentTime = SDL_GetTicks();
  caelumMillis += currentTime - beginTime;
  beginTime = currentTime;

  // draw structures
  shader.use( shader.mesh );

  for( int i = 0; i < structs.length(); ++i ) {
    context.drawBSP( structs[i].str, Mesh::SOLID_BIT );
  }

  currentTime = SDL_GetTicks();
  structsMillis += currentTime - beginTime;
  beginTime = currentTime;

  terra.draw();

  currentTime = SDL_GetTicks();
  terraMillis += currentTime - beginTime;
  beginTime = currentTime;

  // draw objects
  for( int i = 0; i < objects.length(); ++i ) {
    const Object* obj = objects[i].obj;

    if( obj->index == camera.tagged && camera.state != Camera::STRATEGIC ) {
      shader.colour = Colours::TAG;
    }

    context.drawImago( obj, null, Mesh::SOLID_BIT );

    if( obj->index == camera.tagged && camera.state != Camera::STRATEGIC ) {
      shader.colour = Colours::WHITE;
    }
  }

  currentTime = SDL_GetTicks();
  objectsMillis += currentTime - beginTime;
  beginTime = currentTime;

  // draw fragments
  glEnable( GL_BLEND );
  shader.use( shader.mesh );

  glBindTexture( GL_TEXTURE_2D, 0 );
  glUniform1f( param.oz_Specular, 1.0f );

  for( int i = 0; i < frags.length(); ++i ) {
    context.drawFrag( frags[i] );
  }

  // draw transparent parts of objects
  shader.colour = Colours::WHITE;

  currentTime = SDL_GetTicks();
  fragsMillis += currentTime - beginTime;
  beginTime = currentTime;

  glEnable( GL_BLEND );

  for( int i = objects.length() - 1; i >= 0; --i ) {
    const Object* obj = objects[i].obj;

    if( obj->index == camera.tagged && camera.state != Camera::STRATEGIC ) {
      shader.colour = Colours::TAG;
    }

    context.drawImago( obj, null, Mesh::ALPHA_BIT );

    if( obj->index == camera.tagged && camera.state != Camera::STRATEGIC ) {
      shader.colour = Colours::WHITE;
    }
  }

  OZ_GL_CHECK_ERROR();

  currentTime = SDL_GetTicks();
  objectsMillis += currentTime - beginTime;
  beginTime = currentTime;

  terra.drawWater();

  currentTime = SDL_GetTicks();
  terraMillis += currentTime - beginTime;
  beginTime = currentTime;

  // draw structures' alpha parts
  shader.use( shader.mesh );

  for( int i = structs.length() - 1; i >= 0; --i ) {
    context.drawBSP( structs[i].str, Mesh::ALPHA_BIT );
  }

  glDisable( GL_BLEND );

  currentTime = SDL_GetTicks();
  structsMillis += currentTime - beginTime;
  beginTime = currentTime;

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
    for( int i = 0; i < objects.length(); ++i ) {
      glUniform4fv( param.oz_Colour, 1,
                    objects[i].obj->flags & Object::SOLID_BIT ?
                        Colours::CLIP_AABB :
                        Colours::NOCLIP_AABB );
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

  currentTime = SDL_GetTicks();
  miscMillis += currentTime - beginTime;
}

void Render::drawOrbis()
{
  if( isDeferred ) {
    glPushAttrib( GL_VIEWPORT_BIT );
    glViewport( 0, 0, renderWidth, renderHeight );

    uint dbos[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );
    glDrawBuffers( 1, dbos );
  }
  else if( doPostprocess ) {
    glPushAttrib( GL_VIEWPORT_BIT );
    glViewport( 0, 0, renderWidth, renderHeight );

    uint dbos[] = { GL_COLOR_ATTACHMENT0 };

    glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );
    glDrawBuffers( 1, dbos );
  }

  prepareDraw();
  drawGeometry();

  if( isDeferred ) {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glPopAttrib();

  // postprocess
//     tf.ortho();
//     tf.camera = Mat44::ID;
//     tf.applyCamera();
//
//     shader.use( shader.combine );
//
//     shader.setAmbientLight( Colours::ambient );
//     shader.setCaelumLight( caelum.lightDir, Colours::diffuse );
//     shader.updateLights();
//
//     shape.bindVertexArray();
//
//     glBindTexture( GL_TEXTURE_2D, colourBuffer );
//     glActiveTexture( GL_TEXTURE1 );
//     glBindTexture( GL_TEXTURE_2D, normalBuffer );
//
//     shape.fill( 0, 0, camera.width, camera.height );
//
//     glBindTexture( GL_TEXTURE_2D, 0 );
//     glActiveTexture( GL_TEXTURE0 );
//     glBindTexture( GL_TEXTURE_2D, 0 );

    glBindFramebuffer( GL_READ_FRAMEBUFFER, frameBuffer );

    glBlitFramebuffer( 0, 0, renderWidth, renderHeight, 0, 0, camera.width, camera.height,
                       GL_COLOR_BUFFER_BIT, GL_LINEAR );

    glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
  }
  else if( doPostprocess ) {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glPopAttrib();

    tf.ortho();
    tf.camera = Mat44::ID;
    tf.applyCamera();
    shader.use( shader.postprocess );

    glBindTexture( GL_TEXTURE_2D, frameBuffer );
    shape.fill( 0, 0, camera.width, camera.height );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }

  OZ_GL_CHECK_ERROR();
}

void Render::drawUI()
{
  uint beginTime = SDL_GetTicks();

  ui::ui.draw();

  uiMillis += SDL_GetTicks() - beginTime;
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

void Render::sync()
{
  uint beginTime = SDL_GetTicks();

  SDL_GL_SwapBuffers();

  syncMillis += SDL_GetTicks() - beginTime;
}

void Render::toggleFullscreen() const
{
  if( SDL_WM_ToggleFullScreen( surface ) ) {
    ui::mouse.isGrabOn = !ui::mouse.isGrabOn;
    SDL_WM_GrabInput( ui::mouse.isGrabOn ? SDL_GRAB_ON : SDL_GRAB_OFF );
  }
}

void Render::load()
{
  log.println( "Loading Render {" );
  log.indent();

  OZ_GL_CHECK_ERROR();

  ui::ui.load();
  frustum.init();

  structs.alloc( 64 );
  objects.alloc( 8192 );
  frags.alloc( 1024 );

  prepareMillis     = 0;
  caelumMillis      = 0;
  terraMillis       = 0;
  structsMillis     = 0;
  objectsMillis     = 0;
  fragsMillis       = 0;
  miscMillis        = 0;
  postprocessMillis = 0;
  uiMillis          = 0;
  syncMillis        = 0;

  log.unindent();
  log.println( "}" );
}

void Render::unload()
{
  glFinish();

  log.println( "Unloading Render {" );
  log.indent();

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

  log.unindent();
  log.println( "}" );
}

void Render::init( bool isBuild )
{
  log.println( "Initialising Render {" );
  log.indent();

  int  screenX      = config.get( "screen.width", 0 );
  int  screenY      = config.get( "screen.height", 0 );
  int  screenBpp    = config.get( "screen.bpp", 0 );
  bool hasBorder    = config.get( "screen.border", true );
  bool isFullscreen = config.getSet( "screen.full", true );

  log.print( "Creating OpenGL window %dx%d-%d %s ...",
             screenX, screenY, screenBpp, isFullscreen ? "fullscreen" : "windowed" );

  if( ( screenX != 0 || screenY != 0 || screenBpp != 0 ) &&
      SDL_VideoModeOK( screenX, screenY, screenBpp, SDL_OPENGL |
                       ( hasBorder ? 0 : SDL_NOFRAME ) |
                       ( isFullscreen ? SDL_FULLSCREEN : 0 ) ) == 0 )
  {
    log.printEnd( " Mode not supported" );
    throw Exception( "Video mode not supported" );
  }

  surface = SDL_SetVideoMode( screenX, screenY, screenBpp, SDL_OPENGL |
                              ( hasBorder ? 0 : SDL_NOFRAME ) |
                              ( isFullscreen ? SDL_FULLSCREEN : 0 ) );

  if( surface == null ) {
    log.printEnd( " Failed" );
    throw Exception( "Window creation failed" );
  }

  SDL_WM_GrabInput( isFullscreen ? SDL_GRAB_ON : SDL_GRAB_OFF );
  ui::mouse.isGrabOn = SDL_WM_GrabInput( SDL_GRAB_QUERY ) == SDL_GRAB_ON;

  SDL_WM_SetCaption( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION, null );

  screenX   = surface->w;
  screenY   = surface->h;
  screenBpp = surface->format->BitsPerPixel;

  config.getSet( "screen.width", screenX );
  config.getSet( "screen.height", screenY );
  config.getSet( "screen.bpp", screenBpp );

  log.printEnd( " %dx%d-%d ... OK", screenX, screenY, screenBpp );

  SDL_ShowCursor( SDL_FALSE );

  bool isCatalyst  = false;
  bool isGallium   = false;
  bool hasFBO      = false;
  bool hasFloatTex = false;
  bool hasS3TC     = false;
#ifndef OZ_GL_COMPATIBLE
  bool hasVAO      = false;
#endif

  String vendor   = String::cstr( glGetString( GL_VENDOR ) );
  String renderer = String::cstr( glGetString( GL_RENDERER ) );
  String version  = String::cstr( glGetString( GL_VERSION ) );
  String sExtensions = String::cstr( glGetString( GL_EXTENSIONS ) );
  DArray<String> extensions = sExtensions.trim().split( ' ' );

  log.println( "OpenGL vendor: %s", vendor.cstr() );
  log.println( "OpenGL renderer: %s", renderer.cstr() );
  log.println( "OpenGL version: %s", version.cstr() );

  if( log.isVerbose ) {
    log.println( "OpenGL extensions {" );
    log.indent();
  }

  if( strstr( vendor, "ATI" ) != null ) {
    isCatalyst = true;
  }
  if( strstr( renderer, "Gallium" ) != null ) {
    isGallium = true;
  }

  foreach( extension, extensions.citer() ) {
    if( log.isVerbose ) {
      log.println( "%s", extension->cstr() );
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
#ifndef OZ_GL_COMPATIBLE
    if( extension->equals( "GL_ARB_vertex_array_object" ) ) {
      hasVAO = true;
    }
#endif
  }

  if( log.isVerbose ) {
    log.unindent();
    log.println( "}" );
  }

  int major = atoi( version );
  int minor = atoi( version.cstr() + version.index( '.' ) + 1 );

  if( major < 2 || ( major == 2 && minor < 1 ) ) {
    log.println( "Error: at least OpenGL 2.1 with some 3.0 capabilities is required" );
    throw Exception( "Too old OpenGL version" );
  }

  if( !hasFBO ) {
    log.println( "Error: framebuffer object (GL_ARB_framebuffer_object) is not supported" );
    throw Exception( "GL_ARB_framebuffer_object not supported by OpenGL" );
  }
  if( !hasFloatTex ) {
    log.println( "Error: floating-point data texture (GL_ARB_texture_float) is not supported" );
    throw Exception( "GL_ARB_texture_float not supported by OpenGL" );
  }

  if( isGallium ) {
    config.include( "shader.setSamplerIndices", "false" );
  }

  if( isCatalyst ) {
    config.include( "shader.vertexTexture", "false" );
  }

  if( hasS3TC ) {
    shader.hasS3TC = true;
  }

#ifndef OZ_GL_COMPATIBLE
  if( !hasVAO ) {
    log.println( "Error: vertex array object (GL_ARB_vertex_array_object) is not supported" );
    throw Exception( "GL_ARB_vertex_array_object not supported by OpenGL" );
  }
#endif

  if( isBuild ) {
    config.get( "shader.setSamplerIndices", false );

    log.unindent();
    log.println( "}" );

    OZ_GL_CHECK_ERROR();
    return;
  }

  renderWidth     = config.getSet( "render.width",                screenX );
  renderHeight    = config.getSet( "render.height",               screenY );

  isDeferred      = config.getSet( "render.deferred",             false );
  doPostprocess   = config.getSet( "render.postprocess",          false );

  visibilityRange = config.getSet( "render.visibilityRange",      300.0f );
  showBounds      = config.getSet( "render.showBounds",           false );
  showAim         = config.getSet( "render.showAim",              false );

  windPhi         = 0.0f;

  glInit();

  if( isDeferred ) {
    glGenFramebuffers( 1, &frameBuffer );
    glGenTextures( 1, &depthBuffer );
    glGenTextures( 1, &colourBuffer );
    glGenTextures( 1, &normalBuffer );

    glBindTexture( GL_TEXTURE_2D, depthBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, renderWidth, renderHeight, 0,
                  GL_DEPTH_COMPONENT, GL_HALF_FLOAT, null );

    glBindTexture( GL_TEXTURE_2D, colourBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0,
                  GL_RGB, GL_UNSIGNED_BYTE, null );

    glBindTexture( GL_TEXTURE_2D, normalBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, renderWidth, renderHeight, 0,
                  GL_RGB, GL_HALF_FLOAT, null );

    glBindTexture( GL_TEXTURE_2D, 0 );

    glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, depthBuffer,  0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer, 0 );

    if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
      throw Exception( "framebuffer creation failed" );
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  }
  else if( doPostprocess ) {
    glGenFramebuffers( 1, &frameBuffer );
    glGenRenderbuffers( 1, &depthBuffer );
    glGenTextures( 1, &colourBuffer );

    glBindRenderbuffer( GL_RENDERBUFFER, depthBuffer );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderWidth, renderHeight );
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );

    glBindTexture( GL_TEXTURE_2D, colourBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0,
                  GL_RGB, GL_UNSIGNED_BYTE, null );

    glBindTexture( GL_TEXTURE_2D, 0 );

    glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );

    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0 );

    if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
      throw Exception( "framebuffer creation failed" );
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  }

  glEnable( GL_CULL_FACE );
  glDepthFunc( GL_LESS );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glActiveTexture( GL_TEXTURE1 );
  glEnable( GL_TEXTURE_2D );
  glActiveTexture( GL_TEXTURE0 );
  glEnable( GL_TEXTURE_2D );

  shader.init();
  shader.load();
  shape.load();
  camera.init();
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

  log.println( "Shutting down Render {" );
  log.indent();

  if( isDeferred ) {
    glDeleteFramebuffers( 1, &frameBuffer );
    glDeleteTextures( 1, &depthBuffer );
    glDeleteTextures( 1, &colourBuffer );
    glDeleteTextures( 1, &normalBuffer );
  }
  else if( doPostprocess ) {
    glDeleteFramebuffers( 1, &frameBuffer );
    glDeleteRenderbuffers( 1, &depthBuffer );
    glDeleteTextures( 1, &colourBuffer );
    glDeleteTextures( 1, &normalBuffer );
  }

  ui::ui.free();
  shape.unload();
  shader.unload();
  shader.free();

  log.unindent();
  log.println( "}" );
}

}
}
