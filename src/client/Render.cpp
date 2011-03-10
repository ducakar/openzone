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

      if( frustum.isVisible( *obj, factor ) && !( obj->flags & Object::NO_DRAW_BIT ) ) {
        Vec3 relPos = obj->p - camera.p;
        objects.add( ObjectEntry( relPos.sqL(), obj ) );
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
    isUnderWater = collider.hit.inWater;

    Vec4 clearColour;

    if( isUnderWater ) {
      visibility = waterNightVisibility + sky.ratio * ( waterDayVisibility - waterNightVisibility );
      clearColour = Colours::water;
    }
    else {
      visibility = nightVisibility + sky.ratio * ( dayVisibility - nightVisibility );
      clearColour = Colours::sky;
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

    structs.sort();
    objects.sort();

    int firstNearStruct = aBisectPosition( structs + 0, nearDist2, structs.length() );
    int firstNearObject = aBisectPosition( objects + 0, nearDist2, objects.length() );

    // clear buffer
    glClearColor( clearColour.x, clearColour.y, clearColour.z, clearColour.w );
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    hard_assert( !glIsEnabled( GL_BLEND ) );

    // camera transformation
    tf.projection();
    tf.camera = camera.rotTMat;

    currentTime = SDL_GetTicks();
    timer.renderScheduleMillis += currentTime - beginTime;
    beginTime = currentTime;

    sky.draw();

    tf.camera.translate( Point3::ORIGIN - camera.p );

    // set shaders
    Shader::Program programs[] = {
      Shader::MESH_NEAR,
      Shader::MESH_FAR,
      Shader::TERRA,
      Shader::TERRA_WATER,
      Shader::PARTICLES
    };
    foreach( shId, citer( programs, 4 ) ) {
      shader.use( *shId );

      tf.applyCamera();

      shader.setAmbientLight( Colours::GLOBAL_AMBIENT + Colours::ambient );
      shader.setSkyLight( camera.rotTMat * sky.lightDir, Colours::diffuse );
      shader.updateLights();

      glUniform1f( param.oz_NearDistance, Math::sqrt( nearDist2 ) );
      glUniform4fv( param.oz_FogColour, 1, clearColour );
      glUniform1f( param.oz_FogDistance, visibility );
    }

    glEnable( GL_DEPTH_TEST );

    currentTime = SDL_GetTicks();
    timer.renderSkyMillis += currentTime - beginTime;
    beginTime = currentTime;

    hard_assert( !glIsEnabled( GL_BLEND ) );

    // draw structures
    shader.use( Shader::MESH_NEAR );

    for( int i = firstNearStruct; i < structs.length(); ++i ) {
      const Struct* str = structs[i].str;

      context.drawBSP( str, Mesh::SOLID_BIT );
    }

    shader.use( Shader::MESH_FAR );

    for( int i = 0; i < firstNearStruct; ++i ) {
      const Struct* str = structs[i].str;

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
    shader.use( Shader::MESH_FAR );

    for( int i = 0; i < firstNearObject; ++i ) {
      const Object* obj = objects[i].obj;

      if( obj->index == camera.tagged ) {
        glUniform1f( param.oz_Highlight, 1.0f );
      }

      tf.model = Mat44::translation( obj->p - Point3::ORIGIN );
      tf.apply();

      context.drawModel( obj, null );

      if( obj->index == camera.tagged ) {
        glUniform1f( param.oz_Highlight, 0.0f );
      }
    }

    shader.use( Shader::MESH_NEAR );

    for( int i = firstNearObject; i < objects.length(); ++i ) {
      const Object* obj = objects[i].obj;

      if( obj->index == camera.tagged ) {
        glUniform1f( param.oz_Highlight, 1.0f );
      }

      tf.model = Mat44::translation( obj->p - Point3::ORIGIN );
      tf.apply();

      context.drawModel( obj, null );

      if( obj->index == camera.tagged ) {
        glUniform1f( param.oz_Highlight, 0.0f );
      }
    }

    hard_assert( !glIsEnabled( GL_BLEND ) );

    currentTime = SDL_GetTicks();
    timer.renderObjectsMillis += currentTime - beginTime;
    beginTime = currentTime;

    // draw particles
    glEnable( GL_BLEND );

    shader.use( Shader::PARTICLES );

    shape.bindVertexArray();

    for( int i = 0; i < particles.length(); ++i ) {
      const Particle* part = particles[i];

      shape.draw( part );
    }

    glDisable( GL_BLEND );

    hard_assert( glGetError() == GL_NO_ERROR );

    currentTime = SDL_GetTicks();
    timer.renderParticlesMillis += currentTime - beginTime;
    beginTime = currentTime;

    // draw structures' water
    shader.use( Shader::MESH_FAR );

    for( int i = 0; i < firstNearStruct; ++i ) {
      const Struct* str = structs[i].str;

      context.drawBSP( str, Mesh::ALPHA_BIT );
    }

    shader.use( Shader::MESH_NEAR );

    for( int i = firstNearStruct; i < structs.length(); ++i ) {
      const Struct* str = structs[i].str;

      context.drawBSP( str, Mesh::ALPHA_BIT );
    }

    currentTime = SDL_GetTicks();
    timer.renderStructsMillis += currentTime - beginTime;
    beginTime = currentTime;

    terra.drawWater();

    currentTime = SDL_GetTicks();
    timer.renderTerraMillis += currentTime - beginTime;
    beginTime = currentTime;

    shader.use( Shader::SIMPLE );
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

  void Render::drawCommon()
  {
    uint beginTime = SDL_GetTicks();

    ui::ui.draw();

    timer.renderUiMillis += SDL_GetTicks() - beginTime;
  }

  void Render::draw()
  {
    drawOrbis();
    drawCommon();

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
    water.init();
    shape.load();
    sky.load( "sky" );
    terra.load();

    structs.alloc( 64 );
    objects.alloc( 8192 );
    particles.alloc( 1024 );

    log.unindent();
    log.println( "}" );
  }

  void Render::unload()
  {
    log.println( "Unloading Render {" );
    log.indent();

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
    int screenFull = config.getSet( "screen.full", false ) ? SDL_FULLSCREEN : 0;

    log.print( "Creating OpenGL window %dx%d-%d %s ...",
               screenX, screenY, screenBpp, screenFull ? "fullscreen" : "windowed" );

    if( ( screenX != 0 || screenY != 0 || screenBpp != 0 ) &&
        SDL_VideoModeOK( screenX, screenY, screenBpp, SDL_OPENGL | screenFull ) == 0 )
    {
      log.printEnd( " Mode not supported" );
      throw Exception( "Video mode not supported" );
    }

    SDL_Surface* surface = SDL_SetVideoMode( screenX, screenY, screenBpp, SDL_OPENGL | screenFull );

    if( surface == null ) {
      log.printEnd( " Failed" );
      throw Exception( "Window creation failed" );
    }

    screenX   = surface->w;
    screenY   = surface->h;
    screenBpp = surface->format->BitsPerPixel;

    config.getSet( "screen.width", screenX );
    config.getSet( "screen.height", screenY );
    config.getSet( "screen.bpp", screenBpp );

    log.printEnd( " OK, %dx%d-%d", screenX, screenY, screenBpp );

    SDL_ShowCursor( SDL_FALSE );

    bool isVAOSupported = false;
    bool isFBOSupported = false;
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
      if( extension->equals( "GL_EXT_texture_compression_s3tc" ) ) {
        isS3TCSupported = true;
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

    if( !isVAOSupported ) {
      log.println( "Error: vertex array object (GL_ARB_vertex_array_object) is not supported" );
      throw Exception( "GL_ARB_vertex_array_object not supported by OpenGL" );
    }
    if( !isFBOSupported ) {
      log.println( "Error: Frame buffer object (GL_ARB_framebuffer_object) is not supported" );
      throw Exception( "GL_ARB_framebuffer_object not supported by OpenGL" );
    }
    if( !isS3TCSupported ) {
      log.println( "Error: S3 texture compression (GL_EXT_texture_compression_s3tc) is not supported" );
      throw Exception( "GL_EXT_texture_compression_s3tc not supported by OpenGL" );
    }

    nearDist2            = config.getSet( "render.nearDistance",         100.0f );

    dayVisibility        = config.getSet( "render.dayVisibility",        300.0f );
    nightVisibility      = config.getSet( "render.nightVisibility",      300.0f );
    waterDayVisibility   = config.getSet( "render.waterDayVisibility",   12.0f );
    waterNightVisibility = config.getSet( "render.waterNightVisibility", 12.0f );
    particleRadius       = config.getSet( "render.particleRadius",       0.5f );
    showBounds           = config.getSet( "render.showBounds",           false );
    showAim              = config.getSet( "render.showAim",              false );

    nearDist2            *= nearDist2;

#ifdef OZ_WINDOWS
    glUniform1i               = reinterpret_cast<PFNGLUNIFORM1IPROC>               ( SDL_GL_GetProcAddress( "glUniform1i" ) );
    glUniform2i               = reinterpret_cast<PFNGLUNIFORM2IPROC>               ( SDL_GL_GetProcAddress( "glUniform2i" ) );
    glUniform3i               = reinterpret_cast<PFNGLUNIFORM3IPROC>               ( SDL_GL_GetProcAddress( "glUniform3i" ) );
    glUniform4i               = reinterpret_cast<PFNGLUNIFORM4IPROC>               ( SDL_GL_GetProcAddress( "glUniform4i" ) );
    glUniform1iv              = reinterpret_cast<PFNGLUNIFORM1IVPROC>              ( SDL_GL_GetProcAddress( "glUniform1iv" ) );
    glUniform2iv              = reinterpret_cast<PFNGLUNIFORM2IVPROC>              ( SDL_GL_GetProcAddress( "glUniform2iv" ) );
    glUniform3iv              = reinterpret_cast<PFNGLUNIFORM3IVPROC>              ( SDL_GL_GetProcAddress( "glUniform3iv" ) );
    glUniform4iv              = reinterpret_cast<PFNGLUNIFORM4IVPROC>              ( SDL_GL_GetProcAddress( "glUniform4iv" ) );
    glUniform1f               = reinterpret_cast<PFNGLUNIFORM1FPROC>               ( SDL_GL_GetProcAddress( "glUniform1f" ) );
    glUniform2f               = reinterpret_cast<PFNGLUNIFORM2FPROC>               ( SDL_GL_GetProcAddress( "glUniform2f" ) );
    glUniform3f               = reinterpret_cast<PFNGLUNIFORM3FPROC>               ( SDL_GL_GetProcAddress( "glUniform3f" ) );
    glUniform4f               = reinterpret_cast<PFNGLUNIFORM4FPROC>               ( SDL_GL_GetProcAddress( "glUniform4f" ) );
    glUniform1fv              = reinterpret_cast<PFNGLUNIFORM1FVPROC>              ( SDL_GL_GetProcAddress( "glUniform1fv" ) );
    glUniform2fv              = reinterpret_cast<PFNGLUNIFORM2FVPROC>              ( SDL_GL_GetProcAddress( "glUniform2fv" ) );
    glUniform3fv              = reinterpret_cast<PFNGLUNIFORM3FVPROC>              ( SDL_GL_GetProcAddress( "glUniform3fv" ) );
    glUniform4fv              = reinterpret_cast<PFNGLUNIFORM4FVPROC>              ( SDL_GL_GetProcAddress( "glUniform4fv" ) );

    glGenVertexArrays         = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>         ( SDL_GL_GetProcAddress( "glGenVertexArrays" ) );
    glDeleteVertexArrays      = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>      ( SDL_GL_GetProcAddress( "glDeleteVertexArrays" ) );
    glBindVertexArray         = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>         ( SDL_GL_GetProcAddress( "glBindVertexArray" ) );

    glGenBuffers              = reinterpret_cast<PFNGLGENBUFFERSPROC>              ( SDL_GL_GetProcAddress( "glGenBuffers" ) );
    glDeleteBuffers           = reinterpret_cast<PFNGLDELETEBUFFERSPROC>           ( SDL_GL_GetProcAddress( "glDeleteBuffers" ) );
    glBindBuffer              = reinterpret_cast<PFNGLBINDBUFFERPROC>              ( SDL_GL_GetProcAddress( "glBindBuffer" ) );
    glBufferData              = reinterpret_cast<PFNGLBUFFERDATAPROC>              ( SDL_GL_GetProcAddress( "glBufferData" ) );
    glMapBuffer               = reinterpret_cast<PFNGLMAPBUFFERPROC>               ( SDL_GL_GetProcAddress( "glMapBuffer" ) );
    glUnmapBuffer             = reinterpret_cast<PFNGLUNMAPBUFFERPROC>             ( SDL_GL_GetProcAddress( "glUnmapBuffer" ) );

    glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC> ( SDL_GL_GetProcAddress( "glEnableVertexAttribArray" ) );
    glVertexAttribPointer     = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>     ( SDL_GL_GetProcAddress( "glVertexAttribPointer" ) );

    glCreateShader            = reinterpret_cast<PFNGLCREATESHADERPROC>            ( SDL_GL_GetProcAddress( "glCreateShader" ) );
    glDeleteShader            = reinterpret_cast<PFNGLDELETESHADERPROC>            ( SDL_GL_GetProcAddress( "glDeleteShader" ) );
    glShaderSource            = reinterpret_cast<PFNGLSHADERSOURCEPROC>            ( SDL_GL_GetProcAddress( "glShaderSource" ) );
    glCompileShader           = reinterpret_cast<PFNGLCOMPILESHADERPROC>           ( SDL_GL_GetProcAddress( "glCompileShader" ) );
    glGetShaderiv             = reinterpret_cast<PFNGLGETSHADERIVPROC>             ( SDL_GL_GetProcAddress( "glGetShaderiv" ) );
    glGetShaderInfoLog        = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>        ( SDL_GL_GetProcAddress( "glGetShaderInfoLog" ) );
    glCreateProgram           = reinterpret_cast<PFNGLCREATEPROGRAMPROC>           ( SDL_GL_GetProcAddress( "glCreateProgram" ) );
    glDeleteProgram           = reinterpret_cast<PFNGLDELETEPROGRAMPROC>           ( SDL_GL_GetProcAddress( "glDeleteProgram" ) );
    glAttachShader            = reinterpret_cast<PFNGLATTACHSHADERPROC>            ( SDL_GL_GetProcAddress( "glAttachShader" ) );
    glDetachShader            = reinterpret_cast<PFNGLDETACHSHADERPROC>            ( SDL_GL_GetProcAddress( "glDetachShader" ) );
    glLinkProgram             = reinterpret_cast<PFNGLLINKPROGRAMPROC>             ( SDL_GL_GetProcAddress( "glLinkProgram" ) );
    glGetProgramiv            = reinterpret_cast<PFNGLGETPROGRAMIVPROC>            ( SDL_GL_GetProcAddress( "glGetProgramiv" ) );
    glGetProgramInfoLog       = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>       ( SDL_GL_GetProcAddress( "glGetProgramInfoLog" ) );
    glGetUniformLocation      = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>      ( SDL_GL_GetProcAddress( "glGetUniformLocation" ) );
    glUseProgram              = reinterpret_cast<PFNGLUSEPROGRAMPROC>              ( SDL_GL_GetProcAddress( "glUseProgram" ) );

    wglActiveTexture          = reinterpret_cast<PFNGLACTIVETEXTUREPROC>           ( SDL_GL_GetProcAddress( "glActiveTexture" ) );
    wglCompressedTexImage2D   = reinterpret_cast<PFNGLCOMPRESSEDTEXIMAGE2DPROC>    ( SDL_GL_GetProcAddress( "glCompressedTexImage2D" ) );
    wglDrawRangeElements      = reinterpret_cast<PFNGLDRAWRANGEELEMENTSPROC>       ( SDL_GL_GetProcAddress( "glDrawRangeElements" ) );
#endif

    glEnable( GL_CULL_FACE );
    glDepthFunc( GL_LESS );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );
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
