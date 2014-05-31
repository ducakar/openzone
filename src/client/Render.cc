/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

#include <client/Render.hh>

#include <client/Shape.hh>
#include <client/Frustum.hh>
#include <client/Camera.hh>
#include <client/Caelum.hh>
#include <client/Terra.hh>
#include <client/Context.hh>
#include <client/ui/UI.hh>

#include <cstring>

namespace oz
{
namespace client
{

const float Render::WIDE_CULL_FACTOR       = 6.0f;
const float Render::OBJECT_VISIBILITY_COEF = 0.004f;
const float Render::FRAG_VISIBILITY_RANGE2 = 150.0f*150.0f;
const float Render::CELL_RADIUS            =
  ( Cell::SIZE / 2 + Object::MAX_DIM * WIDE_CULL_FACTOR ) * Math::sqrt( 2.0f );
const float Render::EFFECTS_DISTANCE       = 192.0f;

const float Render::NIGHT_FOG_COEFF        = 2.0f;
const float Render::NIGHT_FOG_DIST         = 0.3f;
const float Render::WATER_VISIBILITY       = 32.0f;
const float Render::LAVA_VISIBILITY        = 4.0f;

const float Render::WIND_FACTOR            = 0.0008f;
const float Render::WIND_PHI_INC           = 0.04f;

const int   Render::GLOW_MINIFICATION      = 4;

const Vec4  Render::STRUCT_AABB            = Vec4( 0.20f, 0.50f, 1.00f, 1.00f );
const Vec4  Render::ENTITY_AABB            = Vec4( 1.00f, 0.20f, 0.50f, 1.00f );
const Vec4  Render::SOLID_AABB             = Vec4( 0.50f, 0.80f, 0.20f, 1.00f );
const Vec4  Render::NONSOLID_AABB          = Vec4( 0.70f, 0.80f, 0.90f, 1.00f );

struct Render::DrawEntry
{
  float distance;
  union
  {
    const Struct* str;
    const Object* obj;
  };

  OZ_ALWAYS_INLINE
  explicit DrawEntry() = default;

  OZ_ALWAYS_INLINE
  explicit DrawEntry( float distance_, const Struct* str_ ) :
    distance( distance_ ), str( str_ )
  {}

  OZ_ALWAYS_INLINE
  explicit DrawEntry( float distance_, const Object* obj_ ) :
    distance( distance_ ), obj( obj_ )
  {}

  OZ_ALWAYS_INLINE
  bool operator < ( const DrawEntry& de )
  {
    return distance < de.distance;
  }
};

void Render::effectsMain( void* )
{
  render.effectsRun();
}

void Render::cellEffects( int cellX, int cellY )
{
  const Cell& cell = orbis.cells[cellX][cellY];

  for( const Object& obj : cell.objects ) {
    float radius = EFFECTS_DISTANCE + obj.dim.fastN();
    float dist2  = ( obj.p - camera.p ).sqN();

    if( dist2 > radius*radius ) {
      continue;
    }

    for( const Object::Event& event : obj.events ) {
      if( event.id >= 0 ) {
        continue;
      }

      float scale = min( 1.0f, 64.0f / dist2 );

      if( event.id == Object::EVENT_FLASH ) {
        camera.flash( event.intensity * scale );
      }
      else {
        camera.shake( event.intensity * scale );
      }
    }
  }
}

void Render::effectsRun()
{
  effectsAuxSemaphore.wait();

  while( areEffectsAlive ) {
    Span span = orbis.getInters( camera.p, EFFECTS_DISTANCE );

    for( int x = span.minX ; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        cellEffects( x, y );
      }
    }

    effectsMainSemaphore.post();
    effectsAuxSemaphore.wait();
  }
}

void Render::scheduleCell( int cellX, int cellY )
{
  const Cell& cell = orbis.cells[cellX][cellY];

  for( int i = 0; i < cell.structs.length(); ++i ) {
    if( !drawnStructs.get( cell.structs[i] ) ) {
      drawnStructs.set( cell.structs[i] );

      Struct* str    = orbis.str( cell.structs[i] );
      float   radius = str->dim().fastN();

      if( frustum.isVisible( str->p, radius ) ) {
        float distance = ( str->p - camera.p ).fastN();

        structs.add( DrawEntry( distance, str ) );
      }
    }
  }

  for( const Object& obj : cell.objects ) {
    float radius = obj.dim.fastN();

    if( obj.flags & Object::WIDE_CULL_BIT ) {
      radius *= WIDE_CULL_FACTOR;
    }

    if( frustum.isVisible( obj.p, radius ) ) {
      float distance = ( obj.p - camera.p ).fastN();

      if( radius / ( distance * camera.mag ) >= OBJECT_VISIBILITY_COEF ) {
        objects.add( DrawEntry( distance, &obj ) );
      }
    }
  }

  for( const Frag& frag : cell.frags ) {
    float dist = ( frag.p - camera.p ) * camera.at;

    if( dist <= FRAG_VISIBILITY_RANGE2 && frustum.isVisible( frag.p, FragPool::FRAG_RADIUS ) ) {
      context.drawFrag( &frag );
    }
  }
}

void Render::prepareDraw()
{
  uint currentMicros = Time::uclock();
  uint beginMicros = currentMicros;

  collider.translate( camera.p, Vec3::ZERO );
  shader.medium = collider.hit.medium;

  if( camera.p.z < 0.0f ) {
    shader.fogColour = terra.liquidFogColour;
    visibility = orbis.terra.liquid & Medium::WATER_BIT ? WATER_VISIBILITY : LAVA_VISIBILITY;
  }
  else {
    shader.fogColour = caelum.caelumColour;
    visibility = visibilityRange;
  }

  if( collider.hit.mediumStr != nullptr && ( shader.medium & Medium::LIQUID_MASK ) ) {
    const BSP* bsp = context.getBSP( collider.hit.mediumStr->bsp );

    if( bsp != nullptr ) {
      if( shader.medium & Medium::SEA_BIT ) {
        shader.fogColour = terra.liquidFogColour;
        visibility = orbis.terra.liquid & Medium::WATER_BIT ? WATER_VISIBILITY : LAVA_VISIBILITY;
      }
      else if( shader.medium & Medium::WATER_BIT ) {
        shader.fogColour = bsp->waterFogColour;
        visibility = WATER_VISIBILITY;
      }
      else {
        shader.fogColour = bsp->lavaFogColour;
        visibility = LAVA_VISIBILITY;
      }
    }
  }

  if( camera.p.z < 0.0f || ( shader.medium & Medium::WATER_BIT ) ) {
    float colourRatio = Math::mix( caelum.nightLuminance, 1.0f, caelum.ratio );

    shader.fogColour.x *= colourRatio;
    shader.fogColour.y *= colourRatio;
    shader.fogColour.z *= colourRatio;
  }

  tf.colour = camera.colour * camera.flashColour;

  windPhi = Math::fmod( windPhi + WIND_PHI_INC, Math::TAU );

  // frustum
  camera.maxDist = visibility;

  Span span;
  frustum.update();
  frustum.getExtrems( span, camera.p );

  caelum.update();

  // drawnStructs
  drawnStructs.clearAll();

  float minXCentre = float( ( span.minX - Orbis::CELLS / 2 ) * Cell::SIZE + Cell::SIZE / 2 );
  float minYCentre = float( ( span.minY - Orbis::CELLS / 2 ) * Cell::SIZE + Cell::SIZE / 2 );

  float x = minXCentre;
  for( int i = span.minX; i <= span.maxX; ++i, x = x + Cell::SIZE ) {
    float y = minYCentre;
    for( int j = span.minY; j <= span.maxY; ++j, y = y + Cell::SIZE ) {
      if( frustum.isVisible( x, y, CELL_RADIUS ) ) {
        scheduleCell( i, j );
      }
    }
  }

  structs.sort();
  for( int i = 0; i < structs.length(); ++i ) {
    context.drawBSP( structs[i].str );
  }

  objects.sort();
  for( int i = 0; i < objects.length(); ++i ) {
    context.drawImago( objects[i].obj, nullptr );
  }

  currentMicros = Time::uclock();
  prepareMicros += currentMicros - beginMicros;
}

void Render::drawGeometry()
{
  uint currentMicros = Time::uclock();
  uint beginMicros = currentMicros;

  OZ_GL_CHECK_ERROR();

  currentMicros = Time::uclock();
  swapMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  // camera transformation
  tf.projection();
  tf.camera = camera.rotTMat;
  tf.camera.translate( Point::ORIGIN - camera.p );

  shader.setAmbientLight( Caelum::GLOBAL_AMBIENT_COLOUR + caelum.ambientColour );
  shader.setCaelumLight( caelum.lightDir, caelum.diffuseColour );

  // set shaders
  for( int i = 0; i < liber.shaders.length(); ++i ) {
    shader.program( i );

    tf.applyCamera();
    shader.updateLights();

    glUniform3fv( uniform.fog_colour, 1, shader.fogColour );
    glUniform1f( uniform.fog_distance2, visibility*visibility );
    glUniform4f( uniform.wind, 1.0f, 1.0f, WIND_FACTOR, windPhi );
  }

  currentMicros = Time::uclock();
  miscMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  if( !( shader.medium & Medium::LIQUID_MASK ) && camera.p.z >= 0.0f ) {
    glClear( GL_DEPTH_BUFFER_BIT );

    tf.camera = camera.rotTMat;

    caelum.draw();

    tf.camera.translate( Point::ORIGIN - camera.p );
    tf.applyCamera();
  }
  else {
    glClearColor( shader.fogColour.x, shader.fogColour.y, shader.fogColour.z, 1.0f );
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
  }

  glEnable( GL_DEPTH_TEST );

  currentMicros = Time::uclock();
  caelumMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  Model::drawScheduled( Model::SCENE_QUEUE, Model::SOLID_BIT );

  currentMicros = Time::uclock();
  meshesMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  terra.draw();

  currentMicros = Time::uclock();
  terraMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  terra.drawLiquid();

  currentMicros = Time::uclock();
  terraMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  Model::drawScheduled( Model::SCENE_QUEUE, Model::ALPHA_BIT );
  Model::clearScheduled( Model::SCENE_QUEUE );

  currentMicros = Time::uclock();
  meshesMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  Model::drawScheduled( Model::OVERLAY_QUEUE, Model::SOLID_BIT | Model::ALPHA_BIT );
  Model::clearScheduled( Model::OVERLAY_QUEUE );

  shape.bind();
  shader.program( shader.plain );

  glActiveTexture( Shader::DIFFUSE );
  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

  if( showAim ) {
    Vec3 move = camera.at * 32.0f;
    collider.translate( camera.p, move, camera.botObj );
    move *= collider.hit.ratio;

    shape.colour( 0.0f, 1.0f, 0.0f, 1.0f );
    shape.box( AABB( camera.p + move, Vec3( 0.05f, 0.05f, 0.05f ) ) );
  }

  if( showBounds ) {
    glLineWidth( 1.0f );

    for( int i = 0; i < objects.length(); ++i ) {
      const Object* obj = objects[i].obj;

      shape.colour( obj->flags & Object::SOLID_BIT ? SOLID_AABB : NONSOLID_AABB );
      shape.wireBox( *obj );
    }

    for( int i = 0; i < structs.length(); ++i ) {
      const Struct* str = structs[i].str;

      shape.colour( ENTITY_AABB );

      for( const Entity& entity : str->entities ) {
        Bounds bb = str->toAbsoluteCS( *entity.clazz + entity.offset );
        shape.wireBox( bb.toAABB() );
      }

      shape.colour( STRUCT_AABB );
      shape.wireBox( str->toAABB() );
    }
  }

#ifdef OZ_DYNAMICS
  drawDyn();
#endif

  shape.unbind();

  glDisable( GL_DEPTH_TEST );

  OZ_GL_CHECK_ERROR();

  structs.clear();
  objects.clear();

  currentMicros = Time::uclock();
  miscMicros += currentMicros - beginMicros;
}

void Render::drawOrbis()
{
  if( windowWidth != Window::width() || windowHeight != Window::height() ) {
    resize();
  }

#ifndef GL_ES_VERSION_2_0

  if( isOffscreen ) {
    glViewport( 0, 0, frameWidth, frameHeight );

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mainFrame );

    uint dbos[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
    glDrawBuffers( 2, dbos );
  }

#endif

  prepareDraw();
  drawGeometry();

  uint beginMicros = Time::uclock();

#ifndef GL_ES_VERSION_2_0

  if( isOffscreen ) {
    glViewport( 0, 0, windowWidth, windowHeight );

    tf.ortho( windowWidth, windowHeight );
    tf.camera = Mat4::ID;

    glDisable( GL_CULL_FACE );

    if( shader.doPostprocess ) {
      // Scale glow buffer down.
      glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, minGlowFrame );

      shader.program( shader.plain );
      tf.applyCamera();
      shape.colour( 1.0f, 1.0f, 1.0f );

      glBindTexture( GL_TEXTURE_2D, glowBuffer );
      shape.fill( 0, windowHeight / GLOW_MINIFICATION,
                  frameWidth / GLOW_MINIFICATION, -windowHeight / GLOW_MINIFICATION );

      glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

      // Perform prostprocessing into the screen buffer.
      shader.program( shader.postprocess );
      tf.applyCamera();
      shape.colour( 1.0f, 1.0f, 1.0f );

      glBindTexture( GL_TEXTURE_2D, colourBuffer );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, minGlowBuffer );

      shape.fill( 0, windowHeight, windowWidth, -windowHeight );

      glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
      glActiveTexture( GL_TEXTURE0 );
      glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
    }
    else {
      glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

      shader.program( shader.plain );
      tf.applyCamera();
      shape.colour( 1.0f, 1.0f, 1.0f );

      glBindTexture( GL_TEXTURE_2D, colourBuffer );
      shape.fill( 0, windowHeight, windowWidth, -windowHeight );
      glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
    }

    glEnable( GL_CULL_FACE );
  }

#endif

  postprocessMicros += Time::uclock() - beginMicros;

  OZ_GL_CHECK_ERROR();
}

void Render::drawUI()
{
  uint beginMicros = Time::uclock();

  ui::ui.draw();

  uiMicros += Time::uclock() - beginMicros;
}

void Render::swap()
{
  OZ_NACL_IS_MAIN( false );

  uint beginMicros = Time::uclock();

  Window::swapBuffers();

  swapMicros += Time::uclock() - beginMicros;
}

void Render::update( int flags )
{
  OZ_NACL_IS_MAIN( false );

  if( flags & EFFECTS_BIT ) {
    effectsAuxSemaphore.post();
  }

  MainCall() << [&]
  {
    if( flags & ORBIS_BIT ) {
      drawOrbis();
    }
    if( flags & UI_BIT ) {
      drawUI();
    }
  };

  Model::clearScheduled( Model::SCENE_QUEUE );
  Model::clearScheduled( Model::OVERLAY_QUEUE );

  if( flags & ( ORBIS_BIT | UI_BIT ) ) {
    swap();
  }

  if( flags & EFFECTS_BIT ) {
    effectsMainSemaphore.wait();
  }
}

void Render::resize()
{
  OZ_NACL_IS_MAIN( true );

  windowWidth  = Window::width();
  windowHeight = Window::height();

#ifndef GL_ES_VERSION_2_0

  if( !isOffscreen ) {
    return;
  }

  frameWidth  = Math::lround( float( Window::width()  ) * scale );
  frameHeight = Math::lround( float( Window::height() ) * scale );

  if( mainFrame != 0 ) {
    glDeleteFramebuffersEXT( 1, &mainFrame );
    glDeleteTextures( 1, &colourBuffer );
    glDeleteRenderbuffersEXT( 1, &depthBuffer );
  }
  if( minGlowFrame != 0 ) {
    glDeleteFramebuffersEXT( 1, &minGlowFrame );
    glDeleteTextures( 1, &minGlowBuffer );
    glDeleteTextures( 1, &glowBuffer );
  }

  glGenRenderbuffersEXT( 1, &depthBuffer );
  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, depthBuffer );
  glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, frameWidth, frameHeight );
  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );

  glGenTextures( 1, &colourBuffer );
  glBindTexture( GL_TEXTURE_2D, colourBuffer );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, int( scaleFilter ) );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, int( scaleFilter ) );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                nullptr );

  if( shader.doPostprocess ) {
    glGenTextures( 1, &glowBuffer );
    glBindTexture( GL_TEXTURE_2D, glowBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB,
                  GL_UNSIGNED_BYTE, nullptr );

    glGenTextures( 1, &minGlowBuffer );
    glBindTexture( GL_TEXTURE_2D, minGlowBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB,
                  frameWidth / GLOW_MINIFICATION, frameHeight / GLOW_MINIFICATION, 0, GL_RGB,
                  GL_UNSIGNED_BYTE, nullptr );
  }

  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

  glGenFramebuffersEXT( 1, &mainFrame );
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mainFrame );

  glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
                                depthBuffer );
  glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D,
                             colourBuffer, 0 );
  if( shader.doPostprocess ) {
    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D,
                               glowBuffer, 0 );
  }

  if( glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT ) != GL_FRAMEBUFFER_COMPLETE_EXT ) {
    OZ_ERROR( "Main framebuffer creation failed" );
  }

  if( shader.doPostprocess ) {
    glGenFramebuffersEXT( 1, &minGlowFrame );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, minGlowFrame );

    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D,
                               minGlowBuffer, 0 );

    if( glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT ) != GL_FRAMEBUFFER_COMPLETE_EXT ) {
      OZ_ERROR( "Glow framebuffer creation failed" );
    }
  }

  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

  OZ_GL_CHECK_ERROR();

#endif
}

void Render::load()
{
  OZ_NACL_IS_MAIN( true );

  Log::print( "Loading Render ..." );

  ui::ui.load();

  areEffectsAlive = true;

  effectsThread.start( "effects", Thread::JOINABLE, effectsMain );

  structs.reserve( 64 );
  objects.reserve( 8192 );

  prepareMicros     = 0;
  caelumMicros      = 0;
  terraMicros       = 0;
  meshesMicros      = 0;
  miscMicros        = 0;
  postprocessMicros = 0;
  uiMicros          = 0;
  swapMicros        = 0;

#ifdef OZ_DYNAMICS
  loadDyn();
#endif

  Log::printEnd( " OK" );
}

void Render::unload()
{
  OZ_NACL_IS_MAIN( true );

  Log::print( "Unloading Render ..." );

#ifdef OZ_DYNAMICS
  unloadDyn();
#endif

  glFinish();

  structs.clear();
  structs.trim();

  objects.clear();
  objects.trim();

  areEffectsAlive = false;

  effectsAuxSemaphore.post();
  effectsThread.join();

  ui::ui.unload();

  Log::printEnd( " OK" );
}

void Render::init()
{
  OZ_NACL_IS_MAIN( false );

  Log::println( "Initialising Render {" );
  Log::indent();

  bool isCatalyst  = false;
  bool hasFBO      = false;
  bool hasFloatTex = false;
  bool hasS3TC     = false;

  const char* vendor;
  const char* renderer;
  const char* version;
  const char* glslVersion;
  const char* sExtensions;

  MainCall() << [&]
  {
    vendor      = String::cstr( glGetString( GL_VENDOR ) );
    renderer    = String::cstr( glGetString( GL_RENDERER ) );
    version     = String::cstr( glGetString( GL_VERSION ) );
    glslVersion = String::cstr( glGetString( GL_SHADING_LANGUAGE_VERSION ) );
    sExtensions = String::cstr( glGetString( GL_EXTENSIONS ) );
  };

  if( vendor == nullptr ) {
    OZ_ERROR( "OpenGL failed to initialise" );
  }

  List<String> extensions = String::trim( sExtensions ).split( ' ' );

  Log::println( "OpenGL vendor: %s", vendor );
  Log::println( "OpenGL renderer: %s", renderer );
  Log::println( "OpenGL version: %s", version );
  Log::println( "GLSL version: %s", glslVersion );

  Log::verboseMode = true;

  Log::println( "OpenGL extensions {" );
  Log::indent();

  if( String::beginsWith( vendor, "ATI" ) || String::beginsWith( vendor, "AMD" ) ) {
    isCatalyst = true;
  }
  for( const String& extension : extensions ) {
    Log::println( "%s", extension.cstr() );

    if( extension.equals( "GL_ARB_framebuffer_object" ) ) {
      hasFBO = true;
    }
    if( extension.equals( "GL_ARB_texture_float" ) ) {
      hasFloatTex = true;
    }
    if( extension.equals( "GL_EXT_texture_compression_s3tc" ) ||
        extension.equals( "GL_CHROMIUM_texture_compression_dxt5" ) )
    {
      hasS3TC = true;
    }
  }

#ifdef GL_ES_VERSION_2_0
  hasFBO      = true;
  hasFloatTex = false;
#endif

  Log::unindent();
  Log::println( "}" );

  Log::verboseMode = false;

  if( isCatalyst ) {
    config.include( "shader.vertexTexture", false );
  }
  if( !hasFBO ) {
    OZ_ERROR( "GL_ARB_framebuffer_object not supported by OpenGL" );
  }
  if( !hasFloatTex ) {
    config.include( "shader.vertexTexture", false );
  }
  if( hasS3TC ) {
    shader.hasS3TC = true;
  }

  GL::init();
  shader.init();
  shape.init();

  EnumMap<GLenum> scaleFilterMap = {
    { GL_LINEAR,  "LINEAR"  },
    { GL_NEAREST, "NEAREST" }
  };

  EnumMap<Model::Collation> collationMap = {
    { Model::DEPTH_MAJOR, "DEPTH_MAJOR" },
    { Model::MODEL_MAJOR, "MODEL_MAJOR"  }
  };

#ifdef __native_client__
  const char* sCollation = config.include( "render.collation", "MODEL_MAJOR" ).get( "" );
#else
  const char* sCollation = config.include( "render.collation", "DEPTH_MAJOR" ).get( "" );
#endif
  Model::setCollation( collationMap[ sCollation ] );

  isOffscreen     = config.include( "render.forceFBO",   false ).get( false );
  scale           = config.include( "render.scale",      1.0f ).get( 0.0f );
  scaleFilter     = scaleFilterMap[ config.include( "render.scaleFilter", "LINEAR" ).get( "" ) ];

  visibilityRange = config.include( "render.distance",   350.0f ).get( 0.0f );
  showBounds      = config.include( "render.showBounds", false ).get( false );
  showAim         = config.include( "render.showAim",    false ).get( false );

  isOffscreen     = isOffscreen || shader.doPostprocess || scale != 1.0f;
  windPhi         = 0.0f;

  mainFrame       = 0;
#ifndef GL_ES_VERSION_2_0
  minGlowBuffer   = 0;
#endif

  MainCall() << [&]
  {
    resize();

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_CULL_FACE );
    glEnable( GL_BLEND );
  };

  camera.init();
  ui::ui.init();

  Log::unindent();
  Log::println( "}" );
}

void Render::destroy()
{
  OZ_NACL_IS_MAIN( false );

  Log::println( "Destroying Render {" );
  Log::indent();

  if( mainFrame != 0 ) {
#ifndef GL_ES_VERSION_2_0
    glDeleteFramebuffersEXT( 1, &mainFrame );
    glDeleteTextures( 1, &glowBuffer );
    glDeleteTextures( 1, &colourBuffer );
    glDeleteRenderbuffersEXT( 1, &depthBuffer );
#endif

    mainFrame = 0;
  }

  ui::ui.destroy();
  camera.destroy();
  shape.destroy();
  shader.destroy();

  OZ_GL_CHECK_ERROR();

  Log::unindent();
  Log::println( "}" );
}

Render render;

}
}

#ifdef OZ_DYNAMICS

/*
 * ozDynamics test
 */

#include <ozDynamics/ozDynamics.hh>
#include <common/Timer.hh>
#include <client/Input.hh>

namespace oz
{
namespace client
{

static oz::Space    space;
static oz::Kollider pcollider;
static oz::Physics  physics;

void Render::drawDyn()
{
  if( timer.frameTime > 0.0f ) {
    physics.update( timer.frameTime );
  }

  Vec4 colour = Vec4( 0.0f, 0.0f, 1.0f, 1.0f );

  for( int i = 0; i < space.bodies.length(); ++i ) {
    Body* body = space.bodies[i];

    if( i < 2 && pcollider.overlaps( body, space.bodies[2] ) ) {
      colour = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
    }
    else {
      colour = Vec4( 0.0f, 0.0f, 1.0f, 1.0f );
    }

    body->update();

    shape.colour( colour );
    shape.wireBox( body->bb.toAABB() );
    shape.object( body->p, body->oMat, body->shape() );
  }
}

void Render::loadDyn()
{
  physics.init( &space, &pcollider );

  DBody* body;

//   Compound* c = new Compound();
//   c->add( new Box( Vec3( 1.0f, 1.0f, 1.0f ) ), Vec3( 1.0f, 1.0f, 1.0f ), Mat3::ID );
//   c->add( new Capsule( 1, 1 ), Vec3( -2, 1, 0 ), Mat3::rotationX( Math::TAU / 6.0f ) );

  Box* c = new Box( Vec3( 1, 1, 2 ) );
//   Capsule* c = new Capsule( 1, 1 );

  body = new DBody();
  body->p = Point( 140, 0, 80 );
  body->o = Quat::ID;
  body->setShape( c );
  body->update();
  space.bodies.add( body );

  physics.add( body );

  Box* b = new Box( Vec3( 1, 1, 2 ) );

  body = new DBody();
  body->p = Point( 143, 0, 80 );
  body->o = Quat::ID;
  body->setShape( b );
  body->update();
  space.bodies.add( body );

  physics.add( body );

  Box* p = new Box( Vec3( 10, 10, 1 ) );

  body = new DBody();
  body->p = Point( 142, 0, 75 );
  body->o = Quat::ID;
  body->setShape( p );
  body->update();
  space.bodies.add( body );
}

void Render::unloadDyn()
{
  physics.destroy();
  space.clear();
  Space::deallocate();
}

}
}

#endif
