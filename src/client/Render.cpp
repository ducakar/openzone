/*
 *  Render.cpp
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
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

#include "client/Caelum.hpp"
#include "client/Terra.hpp"
#include "client/BSP.hpp"

#include "client/SMMModel.hpp"
#include "client/SMMVehicleModel.hpp"
#include "client/ExplosionModel.hpp"
#include "client/MD2Model.hpp"
#include "client/MD2WeaponModel.hpp"

#include "client/OpenGL.hpp"

#define OZ_REGISTER_GLFUNC( func, type ) \
  *reinterpret_cast<void**>( &func ) = SDL_GL_GetProcAddress( #func ); \
  if( func == null ) { \
    throw Exception( "Failed to get OpenGL function '" #func "'" ); \
  }

namespace oz
{
namespace client
{

  Render render;

  const float Render::WIDE_CULL_FACTOR = 6.0f;
  const float Render::CELL_WIDE_RADIUS =
      ( Cell::SIZE / 2.0f + AABB::MAX_DIM * WIDE_CULL_FACTOR ) * Math::sqrt( 2.0f );

  const float Render::NIGHT_FOG_COEFF  = 2.0f;
  const float Render::NIGHT_FOG_DIST   = 0.3f;
  const float Render::WATER_VISIBILITY = 8.0f;

#ifndef OZ_TOOLS

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

        structs.add( ObjectEntry( ( p - camera.p ).fastL() - radius, str ) );
      }
    }

    foreach( obj, cell.objects.citer() ) {
      float radius = ( obj->flags & Object::WIDE_CULL_BIT ) ?
          WIDE_CULL_FACTOR * obj->dim.fastL() : obj->dim.fastL();

      if( frustum.isVisible( obj->p, radius ) ) {
        float range = ( obj->p - camera.p ).fastL() - radius;
        objects.add( ObjectEntry( range, obj ) );
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
    OZ_GL_CHECK_ERROR();

    glPushAttrib( GL_VIEWPORT_BIT );
    glViewport( 0, 0, renderWidth, renderHeight );

    uint dbos[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    glBindFramebuffer( GL_FRAMEBUFFER, fbo );
    glDrawBuffers( 2, dbos );

    uint currentTime = SDL_GetTicks();
    uint beginTime = currentTime;

    collider.translate( camera.p, Vec3::ZERO );
    shader.isInWater = collider.hit.inWater;

    windPhi = Math::mod( windPhi + windPhiInc, Math::TAU );

    Vec4 clearColour;

    if( shader.isInWater ) {
      visibility = waterNightVisibility + caelum.ratio * ( waterDayVisibility - waterNightVisibility );
      clearColour = Colours::water;
    }
    else {
      visibility = nightVisibility + caelum.ratio * ( dayVisibility - nightVisibility );
      clearColour = Colours::caelum;
    }

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

    hard_assert( !glIsEnabled( GL_BLEND ) );

    currentTime = SDL_GetTicks();
    timer.renderScheduleMillis += currentTime - beginTime;
    beginTime = currentTime;

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
      glUniform4f( param.oz_Wind, 1.0f, 1.0f, windFactor, windPhi );
    }

    glEnable( GL_DEPTH_TEST );

    currentTime = SDL_GetTicks();
    timer.renderCaelumMillis += currentTime - beginTime;
    beginTime = currentTime;

    hard_assert( !glIsEnabled( GL_BLEND ) );

    // draw structures
    shader.use( shader.mesh );

    for( int i = structs.length() - 1; i >= 0; --i ) {
      const Struct* str = structs[i].str;

      tf.model = Mat44::translation( str->p - Point3::ORIGIN );
      tf.model.rotateZ( float( str->rot ) * Math::TAU / 4.0f );

      context.drawBSP( str, Mesh::SOLID_BIT );
    }

    currentTime = SDL_GetTicks();
    timer.renderStructsMillis += currentTime - beginTime;
    beginTime = currentTime;

    terra.draw();

    currentTime = SDL_GetTicks();
    timer.renderTerraMillis += currentTime - beginTime;
    beginTime = currentTime;

    // draw objects
    for( int i = 0; i < objects.length(); ++i ) {
      const Object* obj = objects[i].obj;

      if( obj->index == camera.tagged && camera.state != Camera::STRATEGIC ) {
        shader.colour = Colours::TAG;
      }

      tf.model = Mat44::translation( obj->p - Point3::ORIGIN );

      context.drawModel( obj, null );

      if( obj->index == camera.tagged && camera.state != Camera::STRATEGIC ) {
        shader.colour = Colours::WHITE;
      }
    }

    hard_assert( !glIsEnabled( GL_BLEND ) );

    currentTime = SDL_GetTicks();
    timer.renderObjectsMillis += currentTime - beginTime;
    beginTime = currentTime;

    // draw particles
    glEnable( GL_BLEND );

    shader.use( shader.mesh );

    glBindTexture( GL_TEXTURE_2D, 0 );
//     glUniform1f( param.oz_Specular, 1.0f );

    shape.bindVertexArray();

    for( int i = 0; i < particles.length(); ++i ) {
      const Particle* part = particles[i];

      tf.model = Mat44::translation( part->p - Point3::ORIGIN );
      tf.model.rotateX( part->rot.x );
      tf.model.rotateY( part->rot.y );
      tf.model.rotateZ( part->rot.z );

      shape.draw( part );
    }

    glDisable( GL_BLEND );

    OZ_GL_CHECK_ERROR();

    currentTime = SDL_GetTicks();
    timer.renderParticlesMillis += currentTime - beginTime;
    beginTime = currentTime;

    terra.drawWater();

    currentTime = SDL_GetTicks();
    timer.renderTerraMillis += currentTime - beginTime;
    beginTime = currentTime;

    // draw structures' alpha parts
    shader.use( shader.mesh );

    for( int i = 0; i < structs.length(); ++i ) {
      const Struct* str = structs[i].str;

      tf.model = Mat44::translation( str->p - Point3::ORIGIN );
      tf.model.rotateZ( float( str->rot ) * Math::TAU / 4.0f );

      context.drawBSP( str, Mesh::ALPHA_BIT );
    }

    currentTime = SDL_GetTicks();
    timer.renderStructsMillis += currentTime - beginTime;
    beginTime = currentTime;

    glBindTexture( GL_TEXTURE_2D, 0 );

    shader.use( shader.plain );
    tf.model = Mat44::ID;

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
                      ( objects[i].obj->flags & Object::SOLID_BIT ) ?
                          Colours::CLIP_AABB : Colours::NOCLIP_AABB );
        shape.wireBox( *objects[i].obj );
        OZ_GL_CHECK_ERROR();
      }

      glUniform4fv( param.oz_Colour, 1, Colours::STRUCTURE_AABB );

      for( int i = 0; i < structs.length(); ++i ) {
        const Struct* str = structs[i].str;

        glUniform4fv( param.oz_Colour, 1, Colours::ENTITY_AABB );

        foreach( entity, citer( str->entities, str->nEntities ) ) {
          Bounds bb = str->toAbsoluteCS( *entity->model + entity->offset );
          shape.wireBox( bb.toAABB() );
          OZ_GL_CHECK_ERROR();
        }

        glUniform4fv( param.oz_Colour, 1, Colours::STRUCTURE_AABB );
        shape.wireBox( str->toAABB() );
        OZ_GL_CHECK_ERROR();
      }
    }

    structs.clear();
    waterStructs.clear();
    objects.clear();
    particles.clear();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glPopAttrib();

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    // postprocess
    tf.ortho();
    tf.camera = Mat44::ID;
    tf.applyCamera();

    shader.use( shader.combine );

    shader.setAmbientLight( Colours::ambient );
    shader.setCaelumLight( caelum.lightDir, Colours::diffuse );
    shader.updateLights();

    shape.bindVertexArray();

    glBindTexture( GL_TEXTURE_2D, colourBuffer );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, normalBuffer );

    shape.fill( 0, 0, camera.width, camera.height );

    glBindTexture( GL_TEXTURE_2D, 0 );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, 0 );

    OZ_GL_CHECK_ERROR();

    currentTime = SDL_GetTicks();
    timer.renderMiscMillis += currentTime - beginTime;
  }

  void Render::drawUI()
  {
    uint beginTime = SDL_GetTicks();

    ui::ui.draw();

    timer.renderUiMillis += SDL_GetTicks() - beginTime;
  }

  void Render::draw( int flags )
  {
    glClear( GL_DEPTH_BUFFER_BIT );

    if( flags & DRAW_ORBIS_BIT ) {
      drawOrbis();
    }
    if( flags & DRAW_UI_BIT ) {
      drawUI();
    }
  }

  void Render::sync() const
  {
    uint beginTime = SDL_GetTicks();

    SDL_GL_SwapBuffers();

    timer.renderSyncMillis += SDL_GetTicks() - beginTime;
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

    shader.load();

    ui::ui.load();

    frustum.init();

    structs.alloc( 64 );
    objects.alloc( 8192 );
    particles.alloc( 1024 );

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

    particles.clear();
    particles.dealloc();

    waterStructs.clear();
    waterStructs.dealloc();

    ui::ui.unload();
    shader.unload();

    log.unindent();
    log.println( "}" );
  }

#endif // OZ_TOOLS

  void Render::init()
  {
    log.println( "Initialising Render {" );
    log.indent();

    fbo  = 0;
    rbo  = 0;

    int  screenX      = config.get( "screen.width", 0 );
    int  screenY      = config.get( "screen.height", 0 );
    int  screenBpp    = config.get( "screen.bpp", 0 );
    bool isFullscreen = config.getSet( "screen.full", true );

    log.print( "Creating OpenGL window %dx%d-%d %s ...",
               screenX, screenY, screenBpp, isFullscreen ? "fullscreen" : "windowed" );

    if( ( screenX != 0 || screenY != 0 || screenBpp != 0 ) &&
        SDL_VideoModeOK( screenX, screenY, screenBpp,
                         SDL_OPENGL | ( isFullscreen ? SDL_FULLSCREEN : 0 ) ) == 0 )
    {
      log.printEnd( " Mode not supported" );
      throw Exception( "Video mode not supported" );
    }

    surface = SDL_SetVideoMode( screenX, screenY, screenBpp,
                                SDL_OPENGL | ( isFullscreen ? SDL_FULLSCREEN : 0 ) );

    if( surface == null ) {
      log.printEnd( " Failed" );
      throw Exception( "Window creation failed" );
    }

#ifndef OZ_TOOLS
    SDL_WM_GrabInput( isFullscreen ? SDL_GRAB_ON : SDL_GRAB_OFF );
    ui::mouse.isGrabOn = SDL_WM_GrabInput( SDL_GRAB_QUERY ) == SDL_GRAB_ON;
#endif

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
    DArray<String> extensions;
    String sExtensions = String::cstr( glGetString( GL_EXTENSIONS ) );
    sExtensions.trim().split( ' ', &extensions );

    log.println( "OpenGL vendor: %s", vendor.cstr() );
    log.println( "OpenGL renderer: %s", renderer.cstr() );
    log.println( "OpenGL version: %s", version.cstr() );
    log.println( "OpenGL extensions {" );
    log.indent();

    if( strstr( vendor, "ATI" ) != null ) {
      isCatalyst = true;
    }
    if( strstr( renderer, "Gallium" ) != null ) {
      isGallium = true;
    }

    foreach( extension, extensions.citer() ) {
      log.println( "%s", extension->cstr() );

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

    log.unindent();
    log.println( "}" );

    int major = atoi( version );
    int minor = atoi( version.cstr() + version.index( '.' ) + 1 );

    if( major < 2 || ( major == 2 && minor < 1 ) ) {
      log.println( "Error: at least OpenGL 2.1 with some 3.0 capabilities is required" );
      throw Exception( "Too old OpenGL version" );
    }

    if( !hasFBO ) {
      log.println( "Error: frame buffer object (GL_ARB_framebuffer_object) is not supported" );
      throw Exception( "GL_ARB_framebuffer_object not supported by OpenGL" );
    }

    if( isGallium ) {
      config.include( "shader.setSamplerIndices", "false" );
    }
    else {
      config.include( "shader.setSamplerIndices", "true" );
    }

    if( !hasFloatTex ) {
      config.add( "shader.vertexTexture", "false" );
    }
    else if( isCatalyst ) {
      config.include( "shader.vertexTexture", "false" );
    }
    else {
      config.include( "shader.vertexTexture", "true" );
    }

    if( !hasS3TC ) {
      config.add( "context.enableS3TC", "false" );
    }
    else {
      config.include( "context.enableS3TC", "true" );
    }

#ifndef OZ_GL_COMPATIBLE
    if( !hasVAO ) {
      log.println( "Error: vertex array object (GL_ARB_vertex_array_object) is not supported" );
      throw Exception( "GL_ARB_vertex_array_object not supported by OpenGL" );
    }
#endif

    renderWidth          = config.getSet( "render.width", screenX );
    renderHeight         = config.getSet( "render.height", screenY );

    nearDist2            = config.getSet( "render.nearDistance",         100.0f );

    dayVisibility        = config.getSet( "render.dayVisibility",        300.0f );
    nightVisibility      = config.getSet( "render.nightVisibility",      300.0f );
    waterDayVisibility   = config.getSet( "render.waterDayVisibility",   32.0f );
    waterNightVisibility = config.getSet( "render.waterNightVisibility", 32.0f );
    particleRadius       = config.getSet( "render.particleRadius",       0.5f );
    showBounds           = config.getSet( "render.showBounds",           false );
    showAim              = config.getSet( "render.showAim",              false );
    windFactor           = config.getSet( "render.windFactor",           0.0008f );
    windPhiInc           = config.getSet( "render.windPhiInc",           0.04f );

    nearDist2            *= nearDist2;
    windPhi              = 0.0f;

#ifdef OZ_MINGW
    OZ_REGISTER_GLFUNC( glUniform1i,               PFNGLUNIFORM1IPROC               );
    OZ_REGISTER_GLFUNC( glUniform2i,               PFNGLUNIFORM2IPROC               );
    OZ_REGISTER_GLFUNC( glUniform3i,               PFNGLUNIFORM3IPROC               );
    OZ_REGISTER_GLFUNC( glUniform4i,               PFNGLUNIFORM4IPROC               );
    OZ_REGISTER_GLFUNC( glUniform1iv,              PFNGLUNIFORM1IVPROC              );
    OZ_REGISTER_GLFUNC( glUniform2iv,              PFNGLUNIFORM2IVPROC              );
    OZ_REGISTER_GLFUNC( glUniform3iv,              PFNGLUNIFORM3IVPROC              );
    OZ_REGISTER_GLFUNC( glUniform4iv,              PFNGLUNIFORM4IVPROC              );
    OZ_REGISTER_GLFUNC( glUniform1f,               PFNGLUNIFORM1FPROC               );
    OZ_REGISTER_GLFUNC( glUniform2f,               PFNGLUNIFORM2FPROC               );
    OZ_REGISTER_GLFUNC( glUniform3f,               PFNGLUNIFORM3FPROC               );
    OZ_REGISTER_GLFUNC( glUniform4f,               PFNGLUNIFORM4FPROC               );
    OZ_REGISTER_GLFUNC( glUniform1fv,              PFNGLUNIFORM1FVPROC              );
    OZ_REGISTER_GLFUNC( glUniform2fv,              PFNGLUNIFORM2FVPROC              );
    OZ_REGISTER_GLFUNC( glUniform3fv,              PFNGLUNIFORM3FVPROC              );
    OZ_REGISTER_GLFUNC( glUniform4fv,              PFNGLUNIFORM4FVPROC              );
    OZ_REGISTER_GLFUNC( glUniformMatrix4fv,        PFNGLUNIFORMMATRIX4FVPROC        );

    OZ_REGISTER_GLFUNC( glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC );
    OZ_REGISTER_GLFUNC( glVertexAttribPointer,     PFNGLVERTEXATTRIBPOINTERPROC     );
    OZ_REGISTER_GLFUNC( glDrawRangeElements,       PFNGLDRAWRANGEELEMENTSPROC       );

    OZ_REGISTER_GLFUNC( glGenBuffers,              PFNGLGENBUFFERSPROC              );
    OZ_REGISTER_GLFUNC( glDeleteBuffers,           PFNGLDELETEBUFFERSPROC           );
    OZ_REGISTER_GLFUNC( glBindBuffer,              PFNGLBINDBUFFERPROC              );
    OZ_REGISTER_GLFUNC( glBufferData,              PFNGLBUFFERDATAPROC              );
    OZ_REGISTER_GLFUNC( glMapBuffer,               PFNGLMAPBUFFERPROC               );
    OZ_REGISTER_GLFUNC( glUnmapBuffer,             PFNGLUNMAPBUFFERPROC             );

    OZ_REGISTER_GLFUNC( glCreateShader,            PFNGLCREATESHADERPROC            );
    OZ_REGISTER_GLFUNC( glDeleteShader,            PFNGLDELETESHADERPROC            );
    OZ_REGISTER_GLFUNC( glShaderSource,            PFNGLSHADERSOURCEPROC            );
    OZ_REGISTER_GLFUNC( glCompileShader,           PFNGLCOMPILESHADERPROC           );
    OZ_REGISTER_GLFUNC( glGetShaderiv,             PFNGLGETSHADERIVPROC             );
    OZ_REGISTER_GLFUNC( glGetShaderInfoLog,        PFNGLGETSHADERINFOLOGPROC        );
    OZ_REGISTER_GLFUNC( glCreateProgram,           PFNGLCREATEPROGRAMPROC           );
    OZ_REGISTER_GLFUNC( glDeleteProgram,           PFNGLDELETEPROGRAMPROC           );
    OZ_REGISTER_GLFUNC( glAttachShader,            PFNGLATTACHSHADERPROC            );
    OZ_REGISTER_GLFUNC( glDetachShader,            PFNGLDETACHSHADERPROC            );
    OZ_REGISTER_GLFUNC( glLinkProgram,             PFNGLLINKPROGRAMPROC             );
    OZ_REGISTER_GLFUNC( glGetProgramiv,            PFNGLGETPROGRAMIVPROC            );
    OZ_REGISTER_GLFUNC( glGetProgramInfoLog,       PFNGLGETPROGRAMINFOLOGPROC       );
    OZ_REGISTER_GLFUNC( glGetUniformLocation,      PFNGLGETUNIFORMLOCATIONPROC      );
    OZ_REGISTER_GLFUNC( glBindAttribLocation,      PFNGLBINDFRAGDATALOCATIONPROC    );
    OZ_REGISTER_GLFUNC( glUseProgram,              PFNGLUSEPROGRAMPROC              );

    OZ_REGISTER_GLFUNC( glActiveTexture,           PFNGLACTIVETEXTUREPROC           );
    OZ_REGISTER_GLFUNC( glCompressedTexImage2D,    PFNGLCOMPRESSEDTEXIMAGE2DPROC    );
    OZ_REGISTER_GLFUNC( glGetCompressedTexImage,   PFNGLGETCOMPRESSEDTEXIMAGEPROC   );
    OZ_REGISTER_GLFUNC( glGenerateMipmap,          PFNGLGENERATEMIPMAPPROC          );

# ifndef OZ_GL_COMPATIBLE
    OZ_REGISTER_GLFUNC( glGenVertexArrays,         PFNGLGENVERTEXARRAYSPROC         );
    OZ_REGISTER_GLFUNC( glDeleteVertexArrays,      PFNGLDELETEVERTEXARRAYSPROC      );
    OZ_REGISTER_GLFUNC( glBindVertexArray,         PFNGLBINDVERTEXARRAYPROC         );
# endif
#endif

    glGenRenderbuffers( 1, &rbo );
    glBindRenderbuffer( GL_RENDERBUFFER, rbo );

    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderWidth, renderHeight );

    glBindRenderbuffer( GL_RENDERBUFFER, 0 );

    glGenTextures( 2, cbos );
    colourBuffer = cbos[0];
    normalBuffer = cbos[1];

    glBindTexture( GL_TEXTURE_2D, colourBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0, GL_RGB,
                  GL_UNSIGNED_BYTE, null );

    glBindTexture( GL_TEXTURE_2D, normalBuffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, renderWidth, renderHeight, 0, GL_RGB,
                  GL_FLOAT, null );

    glBindTexture( GL_TEXTURE_2D, 0 );

    glGenFramebuffers( 1, &fbo );
    glBindFramebuffer( GL_FRAMEBUFFER, fbo );

    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer, 0 );

    if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
      throw Exception( "framebuffer creation failed" );
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glEnable( GL_CULL_FACE );
    glDepthFunc( GL_LESS );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );

#ifndef OZ_TOOLS
    shader.init();
    shape.load();
    camera.init();
    ui::ui.init();
#endif

    OZ_GL_CHECK_ERROR();

    log.unindent();
    log.println( "}" );
  }

  void Render::free()
  {
    log.println( "Shutting down Render {" );
    log.indent();

    if( fbo != 0 ) {
      glDeleteRenderbuffers( 1, &fbo );
      glDeleteTextures( 2, cbos );
      glDeleteRenderbuffers( 1, &rbo );
    }

#ifndef OZ_TOOLS
    ui::ui.free();
    shape.unload();
    shader.free();
#endif

    log.unindent();
    log.println( "}" );
  }

}
}
