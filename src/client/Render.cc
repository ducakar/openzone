/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <stable.hh>
#include <client/Render.hh>

#include <client/Shape.hh>
#include <client/Frustum.hh>
#include <client/Caelum.hh>
#include <client/Terra.hh>
#include <client/Context.hh>
#include <client/Window.hh>

#include <client/SMMImago.hh>
#include <client/SMMVehicleImago.hh>
#include <client/ExplosionImago.hh>
#include <client/MD2WeaponImago.hh>

#include <client/OpenGL.hh>

namespace oz
{
namespace client
{

const float Render::WIDE_CULL_FACTOR       = 6.0f;
const float Render::OBJECT_VISIBILITY_COEF = 0.004f;
const float Render::FRAG_VISIBILITY_RANGE2 = 150.0f*150.0f;
const float Render::CELL_RADIUS            =
  ( Cell::SIZE / 2 + Object::MAX_DIM * WIDE_CULL_FACTOR ) * Math::sqrt( 2.0f );

const float Render::NIGHT_FOG_COEFF        = 2.0f;
const float Render::NIGHT_FOG_DIST         = 0.3f;
const float Render::WATER_VISIBILITY       = 32.0f;
const float Render::LAVA_VISIBILITY        = 4.0f;

const float Render::WIND_FACTOR            = 0.0008f;
const float Render::WIND_PHI_INC           = 0.04f;

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

void Render::scheduleCell( int cellX, int cellY )
{
  const Cell& cell = orbis.cells[cellX][cellY];

  for( int i = 0; i < cell.structs.length(); ++i ) {
    if( !drawnStructs.get( cell.structs[i] ) ) {
      drawnStructs.set( cell.structs[i] );

      Struct* str    = orbis.structs[ cell.structs[i] ];
      float   radius = str->dim().fastN();

      if( frustum.isVisible( str->p, radius ) ) {
        float distance = ( str->p - camera.p ).fastN();

        structs.add( DrawEntry( distance, str ) );
      }
    }
  }

  foreach( obj, cell.objects.citer() ) {
    float radius = obj->dim.fastN();

    if( obj->flags & Object::WIDE_CULL_BIT ) {
      radius *= Object::WIDE_CULL_BIT;
    }

    if( frustum.isVisible( obj->p, radius ) ) {
      float distance = ( obj->p - camera.p ).fastN();

      if( radius / ( distance * camera.mag ) >= OBJECT_VISIBILITY_COEF ) {
        objects.add( DrawEntry( distance, obj ) );
      }
    }
  }

  foreach( frag, cell.frags.citer() ) {
    float dist = ( frag->p - camera.p ) * camera.at;

    if( dist <= FRAG_VISIBILITY_RANGE2 && frustum.isVisible( frag->p, FragPool::FRAG_RADIUS ) ) {
      context.drawFrag( frag );
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
    const BSP* bsp = context.getBSP( collider.hit.mediumStr );

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

  tf.colour        = camera.colour;
  shader.fogColour = tf.colour * shader.fogColour;

  windPhi = Math::fmod( windPhi + WIND_PHI_INC, Math::TAU );

  // frustum
  camera.maxDist = visibility;

  Span span;
  frustum.update();
  frustum.getExtrems( span, camera.p );

  caelum.update();

  // drawnStructs
  if( drawnStructs.length() < orbis.structs.length() ) {
    drawnStructs.deallocate();
    drawnStructs.allocate( orbis.structs.length() );
  }
  drawnStructs.clearAll();

  float minXCentre = float( ( span.minX - Orbis::CELLS / 2 ) * Cell::SIZE + Cell::SIZE / 2 );
  float minYCentre = float( ( span.minY - Orbis::CELLS / 2 ) * Cell::SIZE + Cell::SIZE / 2 );

  float x = minXCentre;
  for( int i = span.minX; i <= span.maxX; ++i, x += Cell::SIZE ) {
    float y = minYCentre;
    for( int j = span.minY; j <= span.maxY; ++j, y += Cell::SIZE ) {
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

  // clear buffer
  glClearColor( shader.fogColour.x, shader.fogColour.y, shader.fogColour.z, shader.fogColour.w );
  glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

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

    glUniform1f( uniform.fog_dist, visibility );
    glUniform4fv( uniform.fog_colour, 1, shader.fogColour );

    glUniform4f( uniform.wind, 1.0f, 1.0f, WIND_FACTOR, windPhi );
  }

  currentMicros = Time::uclock();
  miscMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  if( !( shader.medium & Medium::LIQUID_MASK ) && camera.p.z >= 0.0f ) {
    tf.camera = camera.rotTMat;

    caelum.draw();

    tf.camera.translate( Point::ORIGIN - camera.p );
    tf.applyCamera();
  }

  glEnable( GL_DEPTH_TEST );

  currentMicros = Time::uclock();
  caelumMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  Mesh::drawScheduled( Mesh::SOLID_BIT );

  currentMicros = Time::uclock();
  meshesMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  terra.draw();

  currentMicros = Time::uclock();
  terraMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  terra.drawWater();

  currentMicros = Time::uclock();
  terraMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  Mesh::drawScheduled( Mesh::ALPHA_BIT );
  Mesh::clearScheduled();

  currentMicros = Time::uclock();
  meshesMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  shape.bind();
  shader.program( shader.plain );

  glActiveTexture( GL_TEXTURE0 );
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

      foreach( entity, str->entities.citer() ) {
        Bounds bb = str->toAbsoluteCS( *entity->clazz + entity->offset );
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
  if( isOffscreen ) {
    if( window.width != windowWidth || windowHeight != window.height ) {
      resize();
    }

    glViewport( 0, 0, frameWidth, frameHeight );

#ifdef GL_ES_VERSION_2_0
    glBindFramebuffer( GL_FRAMEBUFFER, mainFrame );
#else
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mainFrame );
#endif

#ifndef GL_ES_VERSION_2_0
//     uint dbos[] = { GL_COLOR_ATTACHMENT0_EXT };
//     glDrawBuffers( 1, dbos );
#endif
  }
  else {
    glViewport( 0, 0, camera.width, camera.height );
  }

  prepareDraw();
  drawGeometry();

  uint beginMicros = Time::uclock();

  if( isOffscreen ) {
    glViewport( 0, 0, windowWidth, windowHeight );

#ifdef GL_ES_VERSION_2_0
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
#else
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
#endif

    tf.ortho( windowWidth, windowHeight );
    tf.camera = Mat44::ID;

    shader.program( doPostprocess ? shader.postprocess : shader.plain );
    tf.applyCamera();
    shape.colour( 1.0f, 1.0f, 1.0f );

    glBindTexture( GL_TEXTURE_2D, colourBuffer );
    shape.fill( 0, 0, windowWidth, windowHeight );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }

  postprocessMicros += Time::uclock() - beginMicros;

  OZ_GL_CHECK_ERROR();
}

void Render::drawUI()
{
  uint beginMicros = Time::uclock();

  ui::ui.draw();

  uiMicros += Time::uclock() - beginMicros;
}

void Render::draw( int flags_ )
{
#ifdef __native_client__
  hard_assert( !NaClPlatform::isMainThread() );
#endif

  flags = flags_;

  OZ_MAIN_CALL( this, {
    if( render.flags & DRAW_ORBIS_BIT ) {
      shader.mode = Shader::SCENE;

      render.drawOrbis();
    }

    if( render.flags & DRAW_UI_BIT ) {
      shader.mode = Shader::UI;

      render.drawUI();
    }
  } )
}

void Render::swap()
{
#ifdef __native_client__
  hard_assert( !NaClPlatform::isMainThread() );
#endif

  uint beginMicros = Time::uclock();

  window.swapBuffers();

  swapMicros += Time::uclock() - beginMicros;
}

void Render::resize()
{
  windowWidth  = window.width;
  windowHeight = window.height;

  if( !isOffscreen ) {
    return;
  }

  frameWidth  = Math::lround( float( window.width  ) * scale );
  frameHeight = Math::lround( float( window.height ) * scale );

  if( mainFrame != 0 ) {
#ifdef GL_ES_VERSION_2_0
    glDeleteFramebuffers( 1, &mainFrame );
    glDeleteTextures( 1, &colourBuffer );
    glDeleteRenderbuffers( 1, &depthBuffer );
#else
    glDeleteFramebuffersEXT( 1, &mainFrame );
    glDeleteTextures( 1, &colourBuffer );
    glDeleteRenderbuffersEXT( 1, &depthBuffer );
#endif
  }

#ifdef GL_ES_VERSION_2_0

  glGenRenderbuffers( 1, &depthBuffer );
  glBindRenderbuffer( GL_RENDERBUFFER, depthBuffer );
  glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, frameWidth, frameHeight );
  glBindRenderbuffer( GL_RENDERBUFFER, 0 );

#else

  glGenRenderbuffersEXT( 1, &depthBuffer );
  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, depthBuffer );
  glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, frameWidth, frameHeight );
  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );

#endif

  glGenTextures( 1, &colourBuffer );
  glBindTexture( GL_TEXTURE_2D, colourBuffer );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaleFilter );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaleFilter );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB,
                GL_UNSIGNED_BYTE, nullptr );

  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

#ifdef GL_ES_VERSION_2_0

  glGenFramebuffers( 1, &mainFrame );
  glBindFramebuffer( GL_FRAMEBUFFER, mainFrame );

  glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer );
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0 );

  if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
    OZ_ERROR( "Main framebuffer creation failed" );
  }

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );

#else

  glGenFramebuffersEXT( 1, &mainFrame );
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, mainFrame );

  glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
                                depthBuffer );
  glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D,
                             colourBuffer, 0 );

  if( glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT ) != GL_FRAMEBUFFER_COMPLETE_EXT ) {
    OZ_ERROR( "Main framebuffer creation failed" );
  }

  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

#endif
}

void Render::load()
{
#ifdef __native_client__
  hard_assert( NaClPlatform::isMainThread() );
#endif

  Log::print( "Loading Render ..." );

  ui::ui.load();

  structs.allocate( 64 );
  objects.allocate( 8192 );

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
#ifdef __native_client__
  hard_assert( NaClPlatform::isMainThread() );
#endif

  Log::print( "Unloading Render ..." );

#ifdef OZ_DYNAMICS
  unloadDyn();
#endif

  glFinish();

  caelum.unload();
  terra.unload();

  drawnStructs.deallocate();

  structs.clear();
  structs.deallocate();

  objects.clear();
  objects.deallocate();

  ui::ui.unload();

  Log::printEnd( " OK" );
}

void Render::init()
{
#ifdef __native_client__
  hard_assert( NaClPlatform::isMainThread() );
#endif

  Log::println( "Initialising Render {" );
  Log::indent();

  bool isMesa7     = false;
  bool isCatalyst  = false;
  bool hasFBO      = false;
  bool hasFloatTex = false;
  bool hasS3TC     = false;

  const char*    vendor       = String::cstr( glGetString( GL_VENDOR ) );
  const char*    renderer     = String::cstr( glGetString( GL_RENDERER ) );
  const char*    version      = String::cstr( glGetString( GL_VERSION ) );
  const char*    glslVersion  = String::cstr( glGetString( GL_SHADING_LANGUAGE_VERSION ) );
  const char*    sExtensions  = String::cstr( glGetString( GL_EXTENSIONS ) );
  DArray<String> extensions   = String::trim( sExtensions ).split( ' ' );

  // glGetString( GL_EXTENSIONS ) generates an error when using OpenGL 3.2+ Core Profile.
  glGetError();

  Log::println( "OpenGL vendor: %s", vendor );
  Log::println( "OpenGL renderer: %s", renderer );
  Log::println( "OpenGL version: %s", version );
  Log::println( "GLSL version: %s", glslVersion );

  Log::verboseMode = true;

  Log::println( "OpenGL extensions {" );
  Log::indent();

  if( strstr( version, "Mesa 7" ) != nullptr ) {
    isMesa7 = true;
  }
  if( strstr( vendor, "ATI" ) != nullptr ) {
    isCatalyst = true;
  }
  foreach( extension, extensions.citer() ) {
    Log::println( "%s", extension->cstr() );

    if( extension->equals( "GL_ARB_framebuffer_object" ) ) {
      hasFBO = true;
    }
    if( extension->equals( "GL_ARB_texture_float" ) ||
        extension->equals( "GL_OES_texture_float" ) )
    {
      hasFloatTex = true;
    }
    if( extension->equals( "GL_EXT_texture_compression_s3tc" ) ||
        extension->equals( "GL_CHROMIUM_texture_compression_dxt5" ) )
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

#ifdef __native_client__
  config.include( "shader.vertexTexture", false );
#endif

  if( isMesa7 ) {
    config.include( "shader.setSamplerMap", false );
  }
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

  glInit();

  String sScaleFilter;

  doPostprocess   = config.include( "render.postprocess", false ).asBool();
  isLowDetail     = config.include( "render.lowDetail",   false ).asBool();

  scale           = config.include( "render.scale",       1.0f ).asFloat();
  sScaleFilter    = config.include( "render.scaleFilter", "LINEAR" ).asString();

  visibilityRange = config.include( "render.distance",    400.0f ).asFloat();
  showBounds      = config.include( "render.showBounds",  false ).asBool();
  showAim         = config.include( "render.showAim",     false ).asBool();

  isOffscreen     = doPostprocess || scale != 1.0f;
  windPhi         = 0.0f;

  if( sScaleFilter.equals( "NEAREST" ) ) {
    scaleFilter = GL_NEAREST;
  }
  else if( sScaleFilter.equals( "LINEAR" ) ) {
    scaleFilter = GL_LINEAR;
  }
  else {
    OZ_ERROR( "render.scaleFilter should be either LINEAR or NEAREST." );
  }

  mainFrame = 0;
  resize();

  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable( GL_CULL_FACE );
  glEnable( GL_BLEND );

  shader.init();
  shape.init();
  camera.init();
  ui::ui.init();

  OZ_GL_CHECK_ERROR();

  Log::unindent();
  Log::println( "}" );
}

void Render::destroy()
{
#ifdef __native_client__
  hard_assert( NaClPlatform::isMainThread() );
#endif

  Log::println( "Destroying Render {" );
  Log::indent();

  if( mainFrame != 0 ) {
#ifdef GL_ES_VERSION_2_0
    glDeleteFramebuffers( 1, &mainFrame );
    glDeleteTextures( 1, &colourBuffer );
    glDeleteRenderbuffers( 1, &depthBuffer );
#else
    glDeleteFramebuffersEXT( 1, &mainFrame );
    glDeleteTextures( 1, &colourBuffer );
    glDeleteRenderbuffersEXT( 1, &depthBuffer );
#endif

    mainFrame = 0;
  }

  ui::ui.destroy();
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
static oz::Collider pcollider;
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
//   c->add( new Box( Vec3( 1.0f, 1.0f, 1.0f ) ), Vec3( 1.0f, 1.0f, 1.0f ), Mat33::ID );
//   c->add( new Capsule( 1, 1 ), Vec3( -2, 1, 0 ), Mat33::rotationX( Math::TAU / 6.0f ) );

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
