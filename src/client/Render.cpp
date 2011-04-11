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

#include "client/Caelum.hpp"
#include "client/Terra.hpp"
#include "client/BSP.hpp"

#include "client/SMMModel.hpp"
#include "client/SMMVehicleModel.hpp"
#include "client/ExplosionModel.hpp"
#include "client/MD2Model.hpp"
#include "client/MD2WeaponModel.hpp"

#define OZ_REGISTER_GLFUNC( func, type ) \
  func = reinterpret_cast<type>( SDL_GL_GetProcAddress( #func ) ); \
  if( func == null ) { \
    throw Exception( "Failed to get OpenGL function '" #func "'" ); \
  }

namespace oz
{
namespace client
{

  Render render;

  const float Render::WIDE_CULL_FACTOR = 6.0f;
  const float Render::CELL_WIDE_RADIUS = Cell::RADIUS + AABB::MAX_DIM * WIDE_CULL_FACTOR;

  const float Render::NIGHT_FOG_COEFF  = 2.0f;
  const float Render::NIGHT_FOG_DIST   = 0.3f;
  const float Render::WATER_VISIBILITY = 8.0f;

  void Render::scheduleCell( int cellX, int cellY )
  {
    const Cell& cell = orbis.cells[cellX][cellY];

    for( int i = 0; i < cell.structs.length(); ++i ) {
      Struct* str = orbis.structs[ cell.structs[i] ];

      if( !drawnStructs.get( cell.structs[i] ) && frustum.isVisible( *str ) ) {
        drawnStructs.set( cell.structs[i] );

        Vec3 relPos = str->mins + 0.5f * ( str->maxs - str->mins ) - camera.p;
        structs.add( ObjectEntry( relPos.sqL(), str ) );
      }
    }

    foreach( obj, cell.objects.citer() ) {
      float factor = ( obj->flags & Object::WIDE_CULL_BIT ) ? WIDE_CULL_FACTOR : 1.0f;
      float radius = factor * obj->dim.fastL();

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
    hard_assert( glGetError() == GL_NO_ERROR );

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
        if( frustum.isVisible( x, y, CELL_WIDE_RADIUS  ) ) {
          scheduleCell( i, j );
        }
      }
    }

    structs.sort();
    objects.sort();

    // clear buffer
    glClearColor( clearColour.x, clearColour.y, clearColour.z, clearColour.w );
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    hard_assert( !glIsEnabled( GL_BLEND ) );

    currentTime = SDL_GetTicks();
    timer.renderScheduleMillis += currentTime - beginTime;
    beginTime = currentTime;

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

    // set shaders
    for( int i = 0; i < translator.shaders.length(); ++i ) {
      if( i == shader.ui ) {
        continue;
      }

      shader.use( i );

      tf.applyCamera();

      shader.setAmbientLight( Colours::GLOBAL_AMBIENT + Colours::ambient );
      shader.setCaelumLight( caelum.lightDir, Colours::diffuse );
      shader.updateLights();

      glUniform1f( param.oz_Specular, 1.0f );

      glUniform1f( param.oz_Fog_start, shader.isInWater ? 0.0f : 100.0f );
      glUniform1f( param.oz_Fog_end, visibility );
      glUniform4fv( param.oz_Fog_colour, 1, clearColour );

      glUniform4f( param.oz_Wind, 1.0f, 1.0f, windFactor, windPhi );
    }

    glEnable( GL_DEPTH_TEST );

    currentTime = SDL_GetTicks();
    timer.renderCaelumMillis += currentTime - beginTime;
    beginTime = currentTime;

    hard_assert( !glIsEnabled( GL_BLEND ) );

    // draw structures
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

    shader.use( particleShaderId );

    glUniform1f( param.oz_Specular, 1.0f );

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

    hard_assert( glGetError() == GL_NO_ERROR );

    currentTime = SDL_GetTicks();
    timer.renderParticlesMillis += currentTime - beginTime;
    beginTime = currentTime;

    terra.drawWater();

    currentTime = SDL_GetTicks();
    timer.renderTerraMillis += currentTime - beginTime;
    beginTime = currentTime;

    // draw structures' alpha parts
    for( int i = 0; i < structs.length(); ++i ) {
      const Struct* str = structs[i].str;

      tf.model = Mat44::translation( str->p - Point3::ORIGIN );
      tf.model.rotateZ( float( str->rot ) * Math::TAU / 4.0f );

      context.drawBSP( str, Mesh::ALPHA_BIT );
    }

    currentTime = SDL_GetTicks();
    timer.renderStructsMillis += currentTime - beginTime;
    beginTime = currentTime;

    shader.use( simpleShaderId );
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
        hard_assert( glGetError() == GL_NO_ERROR );
      }

      glUniform4fv( param.oz_Colour, 1, Colours::STRUCTURE_AABB );

      for( int i = 0; i < structs.length(); ++i ) {
        const Struct* str = structs[i].str;

        glUniform4fv( param.oz_Colour, 1, Colours::ENTITY_AABB );

        foreach( entity, citer( str->entities, str->nEntities ) ) {
          Bounds bb = str->toAbsoluteCS( *entity->model + entity->offset );
          shape.wireBox( bb.toAABB() );
          hard_assert( glGetError() == GL_NO_ERROR );
        }

        glUniform4fv( param.oz_Colour, 1, Colours::STRUCTURE_AABB );
        shape.wireBox( str->toAABB() );
        hard_assert( glGetError() == GL_NO_ERROR );
      }
    }

    structs.clear();
    waterStructs.clear();
    objects.clear();
    particles.clear();

    glDisable( GL_DEPTH_TEST );

    hard_assert( glGetError() == GL_NO_ERROR );

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

  void Render::load()
  {
    log.println( "Loading Render {" );
    log.indent();

    hard_assert( glGetError() == GL_NO_ERROR );

    shader.load();

    ui::ui.load();

    frustum.init();
    terra.load();
    caelum.load( "caelum" );

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

  void Render::init()
  {
    log.println( "Initialising Render {" );
    log.indent();

    int screenX    = config.get( "screen.width", 0 );
    int screenY    = config.get( "screen.height", 0 );
    int screenBpp  = config.get( "screen.bpp", 0 );
    int screenFull = config.getSet( "screen.full", true ) ? SDL_FULLSCREEN : 0;

    log.print( "Creating OpenGL window %dx%d-%d %s ...",
               screenX, screenY, screenBpp, screenFull ? "fullscreen" : "windowed" );

    if( ( screenX != 0 || screenY != 0 || screenBpp != 0 ) &&
        SDL_VideoModeOK( screenX, screenY, screenBpp, SDL_OPENGL | screenFull ) == 0 )
    {
      log.printEnd( " Mode not supported" );
      throw Exception( "Video mode not supported" );
    }

    surface = SDL_SetVideoMode( screenX, screenY, screenBpp, SDL_OPENGL | screenFull );

    if( surface == null ) {
      log.printEnd( " Failed" );
      throw Exception( "Window creation failed" );
    }

    SDL_WM_SetCaption( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION, null );

    screenX   = surface->w;
    screenY   = surface->h;
    screenBpp = surface->format->BitsPerPixel;

    config.getSet( "screen.width", screenX );
    config.getSet( "screen.height", screenY );
    config.getSet( "screen.bpp", screenBpp );

    log.printEnd( " %dx%d-%d ... OK", screenX, screenY, screenBpp );

    SDL_ShowCursor( SDL_FALSE );

    bool isVAOSupported = false;
    bool isFBOSupported = false;
    bool isFloatTexSupported = false;
    bool isS3TCSupported = false;

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
        isVAOSupported = true;
      }
      if( extension->equals( "GL_ARB_framebuffer_object" ) ) {
        isFBOSupported = true;
      }
      if( extension->equals( "GL_ARB_texture_float" ) ) {
        isFloatTexSupported = true;
      }
      if( extension->equals( "GL_EXT_texture_compression_s3tc" ) ) {
        isS3TCSupported = true;
      }
    }

    log.unindent();
    log.println( "}" );

    int major = atoi( version );
    int minor = atoi( version.cstr() + version.index( '.' ) + 1 );

    if( major < 2 || ( major == 2 && minor < 1 ) ) {
      log.println( "Error: at least OpenGL 2.1 required" );
      throw Exception( "Too old OpenGL version" );
    }

    if( !isVAOSupported ) {
      log.println( "Error: vertex array object (GL_ARB_vertex_array_object) is not supported" );
      throw Exception( "GL_ARB_vertex_array_object not supported by OpenGL" );
    }
    if( !isFBOSupported ) {
      log.println( "Error: Framebuffer object (GL_ARB_framebuffer_object) is not supported" );
      throw Exception( "GL_ARB_framebuffer_object not supported by OpenGL" );
    }
    if( !isFloatTexSupported ) {
      log.println( "Error: Floating point texture pixels (GL_ARB_texture_float) not supported" );
      throw Exception( "GL_ARB_texture_float not supported by OpenGL" );
    }
    if( !isS3TCSupported ) {
      log.println( "Error: S3 texture compression (GL_EXT_texture_compression_s3tc) is not supported" );
      throw Exception( "GL_EXT_texture_compression_s3tc not supported by OpenGL" );
    }

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

    simpleShaderId       = translator.shaderIndex( "simple" );
    particleShaderId     = translator.shaderIndex( "particles" );

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

    OZ_REGISTER_GLFUNC( glGenVertexArrays,         PFNGLGENVERTEXARRAYSPROC         );
    OZ_REGISTER_GLFUNC( glDeleteVertexArrays,      PFNGLDELETEVERTEXARRAYSPROC      );
    OZ_REGISTER_GLFUNC( glBindVertexArray,         PFNGLBINDVERTEXARRAYPROC         );

    OZ_REGISTER_GLFUNC( glGenBuffers,              PFNGLGENBUFFERSPROC              );
    OZ_REGISTER_GLFUNC( glDeleteBuffers,           PFNGLDELETEBUFFERSPROC           );
    OZ_REGISTER_GLFUNC( glBindBuffer,              PFNGLBINDBUFFERPROC              );
    OZ_REGISTER_GLFUNC( glBufferData,              PFNGLBUFFERDATAPROC              );
    OZ_REGISTER_GLFUNC( glMapBuffer,               PFNGLMAPBUFFERPROC               );
    OZ_REGISTER_GLFUNC( glUnmapBuffer,             PFNGLUNMAPBUFFERPROC             );

    OZ_REGISTER_GLFUNC( glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC );
    OZ_REGISTER_GLFUNC( glVertexAttribPointer,     PFNGLVERTEXATTRIBPOINTERPROC     );

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
    OZ_REGISTER_GLFUNC( glBindFragDataLocation,    PFNGLBINDFRAGDATALOCATIONPROC    );
    OZ_REGISTER_GLFUNC( glUseProgram,              PFNGLUSEPROGRAMPROC              );

    OZ_REGISTER_GLFUNC( glActiveTexture,           PFNGLACTIVETEXTUREPROC           );
    OZ_REGISTER_GLFUNC( glCompressedTexImage2D,    PFNGLCOMPRESSEDTEXIMAGE2DPROC    );
    OZ_REGISTER_GLFUNC( glDrawRangeElements,       PFNGLDRAWRANGEELEMENTSPROC       );
#endif

    glEnable( GL_CULL_FACE );
    glDepthFunc( GL_LESS );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );

    shader.init();
    shape.load();
    camera.init();
    ui::ui.init();

    hard_assert( glGetError() == GL_NO_ERROR );

    log.unindent();
    log.println( "}" );
  }

  void Render::free()
  {
    log.println( "Shutting down Render {" );
    log.indent();

    ui::ui.free();
    shape.unload();
    shader.free();

    log.unindent();
    log.println( "}" );
  }

}
}
