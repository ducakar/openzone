/*
 *  MD2.cpp
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/MD2.hpp"

#include "client/Context.hpp"
#include "client/Colours.hpp"
#include "client/Compiler.hpp"

#define FOURCC( a, b, c, d ) \
  ( ( a ) | ( ( b ) << 8 ) | ( ( c ) << 16 ) | ( ( d ) << 24 ) )

namespace oz
{
namespace client
{

  struct MD2Header
  {
    int id;
    int version;

    int skinWidth;
    int skinHeight;
    int frameSize;

    int nSkins;
    int nVerts;
    int nTexCoords;
    int nTris;
    int nGlCmds;
    int nFrames;

    int offSkins;
    int offTexCoords;
    int offTris;
    int offFrames;
    int offGLCmds;
    int offEnd;
  };

  struct MD2Vertex
  {
    ubyte p[3];
    ubyte normal;
  };

  struct MD2Frame
  {
    float     scale[3];
    float     translate[3];
    char      name[16];
    MD2Vertex verts[1];
  };

  struct MD2Triangle
  {
    short vertices[3];
    short texCoords[3];
  };

  static const Vec3 NORMALS[] =
  {
    Vec3( -0.000000f, -0.525731f,  0.850651f ),
    Vec3( -0.238856f, -0.442863f,  0.864188f ),
    Vec3( -0.000000f, -0.295242f,  0.955423f ),
    Vec3( -0.500000f, -0.309017f,  0.809017f ),
    Vec3( -0.262866f, -0.162460f,  0.951056f ),
    Vec3( -0.000000f,  0.000000f,  1.000000f ),
    Vec3( -0.850651f,  0.000000f,  0.525731f ),
    Vec3( -0.716567f, -0.147621f,  0.681718f ),
    Vec3( -0.716567f,  0.147621f,  0.681718f ),
    Vec3( -0.525731f,  0.000000f,  0.850651f ),
    Vec3( -0.500000f,  0.309017f,  0.809017f ),
    Vec3( -0.000000f,  0.525731f,  0.850651f ),
    Vec3( -0.000000f,  0.295242f,  0.955423f ),
    Vec3( -0.238856f,  0.442863f,  0.864188f ),
    Vec3( -0.262866f,  0.162460f,  0.951056f ),
    Vec3( -0.147621f, -0.681718f,  0.716567f ),
    Vec3( -0.309017f, -0.809017f,  0.500000f ),
    Vec3( -0.425325f, -0.587785f,  0.688191f ),
    Vec3( -0.525731f, -0.850651f,  0.000000f ),
    Vec3( -0.442863f, -0.864188f,  0.238856f ),
    Vec3( -0.681718f, -0.716567f,  0.147621f ),
    Vec3( -0.587785f, -0.688191f,  0.425325f ),
    Vec3( -0.809017f, -0.500000f,  0.309017f ),
    Vec3( -0.864188f, -0.238856f,  0.442863f ),
    Vec3( -0.688191f, -0.425325f,  0.587785f ),
    Vec3( -0.681718f, -0.716567f, -0.147621f ),
    Vec3( -0.809017f, -0.500000f, -0.309017f ),
    Vec3( -0.850651f, -0.525731f,  0.000000f ),
    Vec3( -0.850651f,  0.000000f, -0.525731f ),
    Vec3( -0.864188f, -0.238856f, -0.442863f ),
    Vec3( -0.955423f,  0.000000f, -0.295242f ),
    Vec3( -0.951056f, -0.262866f, -0.162460f ),
    Vec3( -1.000000f,  0.000000f,  0.000000f ),
    Vec3( -0.955423f,  0.000000f,  0.295242f ),
    Vec3( -0.951056f, -0.262866f,  0.162460f ),
    Vec3( -0.864188f,  0.238856f,  0.442863f ),
    Vec3( -0.951056f,  0.262866f,  0.162460f ),
    Vec3( -0.809017f,  0.500000f,  0.309017f ),
    Vec3( -0.864188f,  0.238856f, -0.442863f ),
    Vec3( -0.951056f,  0.262866f, -0.162460f ),
    Vec3( -0.809017f,  0.500000f, -0.309017f ),
    Vec3( -0.525731f,  0.850651f,  0.000000f ),
    Vec3( -0.681718f,  0.716567f,  0.147621f ),
    Vec3( -0.681718f,  0.716567f, -0.147621f ),
    Vec3( -0.850651f,  0.525731f,  0.000000f ),
    Vec3( -0.688191f,  0.425325f,  0.587785f ),
    Vec3( -0.442863f,  0.864188f,  0.238856f ),
    Vec3( -0.587785f,  0.688191f,  0.425325f ),
    Vec3( -0.309017f,  0.809017f,  0.500000f ),
    Vec3( -0.147621f,  0.681718f,  0.716567f ),
    Vec3( -0.425325f,  0.587785f,  0.688191f ),
    Vec3( -0.295242f,  0.955423f,  0.000000f ),
    Vec3( -0.000000f,  1.000000f,  0.000000f ),
    Vec3( -0.162460f,  0.951056f,  0.262866f ),
    Vec3(  0.525731f,  0.850651f,  0.000000f ),
    Vec3(  0.295242f,  0.955423f,  0.000000f ),
    Vec3(  0.442863f,  0.864188f,  0.238856f ),
    Vec3(  0.162460f,  0.951056f,  0.262866f ),
    Vec3(  0.309017f,  0.809017f,  0.500000f ),
    Vec3(  0.147621f,  0.681718f,  0.716567f ),
    Vec3( -0.000000f,  0.850651f,  0.525731f ),
    Vec3( -0.442863f,  0.864188f, -0.238856f ),
    Vec3( -0.309017f,  0.809017f, -0.500000f ),
    Vec3( -0.162460f,  0.951056f, -0.262866f ),
    Vec3( -0.000000f,  0.525731f, -0.850651f ),
    Vec3( -0.147621f,  0.681718f, -0.716567f ),
    Vec3(  0.147621f,  0.681718f, -0.716567f ),
    Vec3( -0.000000f,  0.850651f, -0.525731f ),
    Vec3(  0.309017f,  0.809017f, -0.500000f ),
    Vec3(  0.442863f,  0.864188f, -0.238856f ),
    Vec3(  0.162460f,  0.951056f, -0.262866f ),
    Vec3( -0.716567f,  0.147621f, -0.681718f ),
    Vec3( -0.500000f,  0.309017f, -0.809017f ),
    Vec3( -0.688191f,  0.425325f, -0.587785f ),
    Vec3( -0.238856f,  0.442863f, -0.864188f ),
    Vec3( -0.425325f,  0.587785f, -0.688191f ),
    Vec3( -0.587785f,  0.688191f, -0.425325f ),
    Vec3( -0.716567f, -0.147621f, -0.681718f ),
    Vec3( -0.500000f, -0.309017f, -0.809017f ),
    Vec3( -0.525731f,  0.000000f, -0.850651f ),
    Vec3( -0.000000f, -0.525731f, -0.850651f ),
    Vec3( -0.238856f, -0.442863f, -0.864188f ),
    Vec3( -0.000000f, -0.295242f, -0.955423f ),
    Vec3( -0.262866f, -0.162460f, -0.951056f ),
    Vec3( -0.000000f,  0.000000f, -1.000000f ),
    Vec3( -0.000000f,  0.295242f, -0.955423f ),
    Vec3( -0.262866f,  0.162460f, -0.951056f ),
    Vec3(  0.238856f, -0.442863f, -0.864188f ),
    Vec3(  0.500000f, -0.309017f, -0.809017f ),
    Vec3(  0.262866f, -0.162460f, -0.951056f ),
    Vec3(  0.850651f,  0.000000f, -0.525731f ),
    Vec3(  0.716567f, -0.147621f, -0.681718f ),
    Vec3(  0.716567f,  0.147621f, -0.681718f ),
    Vec3(  0.525731f,  0.000000f, -0.850651f ),
    Vec3(  0.500000f,  0.309017f, -0.809017f ),
    Vec3(  0.238856f,  0.442863f, -0.864188f ),
    Vec3(  0.262866f,  0.162460f, -0.951056f ),
    Vec3(  0.864188f,  0.238856f, -0.442863f ),
    Vec3(  0.809017f,  0.500000f, -0.309017f ),
    Vec3(  0.688191f,  0.425325f, -0.587785f ),
    Vec3(  0.681718f,  0.716567f, -0.147621f ),
    Vec3(  0.587785f,  0.688191f, -0.425325f ),
    Vec3(  0.425325f,  0.587785f, -0.688191f ),
    Vec3(  0.955423f,  0.000000f, -0.295242f ),
    Vec3(  1.000000f,  0.000000f,  0.000000f ),
    Vec3(  0.951056f,  0.262866f, -0.162460f ),
    Vec3(  0.850651f,  0.000000f,  0.525731f ),
    Vec3(  0.955423f,  0.000000f,  0.295242f ),
    Vec3(  0.864188f,  0.238856f,  0.442863f ),
    Vec3(  0.951056f,  0.262866f,  0.162460f ),
    Vec3(  0.809017f,  0.500000f,  0.309017f ),
    Vec3(  0.681718f,  0.716567f,  0.147621f ),
    Vec3(  0.850651f,  0.525731f,  0.000000f ),
    Vec3(  0.864188f, -0.238856f, -0.442863f ),
    Vec3(  0.809017f, -0.500000f, -0.309017f ),
    Vec3(  0.951056f, -0.262866f, -0.162460f ),
    Vec3(  0.525731f, -0.850651f,  0.000000f ),
    Vec3(  0.681718f, -0.716567f, -0.147621f ),
    Vec3(  0.681718f, -0.716567f,  0.147621f ),
    Vec3(  0.850651f, -0.525731f,  0.000000f ),
    Vec3(  0.809017f, -0.500000f,  0.309017f ),
    Vec3(  0.864188f, -0.238856f,  0.442863f ),
    Vec3(  0.951056f, -0.262866f,  0.162460f ),
    Vec3(  0.442863f, -0.864188f,  0.238856f ),
    Vec3(  0.309017f, -0.809017f,  0.500000f ),
    Vec3(  0.587785f, -0.688191f,  0.425325f ),
    Vec3(  0.147621f, -0.681718f,  0.716567f ),
    Vec3(  0.238856f, -0.442863f,  0.864188f ),
    Vec3(  0.425325f, -0.587785f,  0.688191f ),
    Vec3(  0.500000f, -0.309017f,  0.809017f ),
    Vec3(  0.716567f, -0.147621f,  0.681718f ),
    Vec3(  0.688191f, -0.425325f,  0.587785f ),
    Vec3(  0.262866f, -0.162460f,  0.951056f ),
    Vec3(  0.238856f,  0.442863f,  0.864188f ),
    Vec3(  0.262866f,  0.162460f,  0.951056f ),
    Vec3(  0.500000f,  0.309017f,  0.809017f ),
    Vec3(  0.716567f,  0.147621f,  0.681718f ),
    Vec3(  0.525731f,  0.000000f,  0.850651f ),
    Vec3(  0.688191f,  0.425325f,  0.587785f ),
    Vec3(  0.425325f,  0.587785f,  0.688191f ),
    Vec3(  0.587785f,  0.688191f,  0.425325f ),
    Vec3( -0.295242f, -0.955423f,  0.000000f ),
    Vec3( -0.162460f, -0.951056f,  0.262866f ),
    Vec3( -0.000000f, -1.000000f,  0.000000f ),
    Vec3( -0.000000f, -0.850651f,  0.525731f ),
    Vec3(  0.295242f, -0.955423f,  0.000000f ),
    Vec3(  0.162460f, -0.951056f,  0.262866f ),
    Vec3( -0.442863f, -0.864188f, -0.238856f ),
    Vec3( -0.162460f, -0.951056f, -0.262866f ),
    Vec3( -0.309017f, -0.809017f, -0.500000f ),
    Vec3(  0.442863f, -0.864188f, -0.238856f ),
    Vec3(  0.162460f, -0.951056f, -0.262866f ),
    Vec3(  0.309017f, -0.809017f, -0.500000f ),
    Vec3( -0.147621f, -0.681718f, -0.716567f ),
    Vec3(  0.147621f, -0.681718f, -0.716567f ),
    Vec3( -0.000000f, -0.850651f, -0.525731f ),
    Vec3( -0.587785f, -0.688191f, -0.425325f ),
    Vec3( -0.425325f, -0.587785f, -0.688191f ),
    Vec3( -0.688191f, -0.425325f, -0.587785f ),
    Vec3(  0.688191f, -0.425325f, -0.587785f ),
    Vec3(  0.425325f, -0.587785f, -0.688191f ),
    Vec3(  0.587785f, -0.688191f, -0.425325f )
  };

  MD2::AnimInfo MD2::animList[] =
  {
    // first, last, fps, repeat
    {   0,  39,  9.0f, 1 },   // STAND
    {  40,  45, 10.0f, 1 },   // RUN
    {  46,  53, 16.0f, 1 },   // ATTACK
    {  54,  57,  7.0f, 1 },   // PAIN_A
    {  58,  61,  7.0f, 1 },   // PAIN_B
    {  62,  65,  7.0f, 1 },   // PAIN_C
    {  67,  67,  9.0f, 0 },   // JUMP
    {  72,  83,  7.0f, 0 },   // FLIP
    {  84,  94,  7.0f, 0 },   // SALUTE
    {  95, 111, 10.0f, 0 },   // FALLBACK
    { 112, 122,  7.0f, 0 },   // WAVE
    { 123, 134,  6.0f, 0 },   // POINT
    { 135, 153, 10.0f, 1 },   // CROUCH_STAND
    { 154, 159,  7.0f, 1 },   // CROUCH_WALK
    { 160, 168, 18.0f, 1 },   // CROUCH_ATTACK
    { 169, 172,  7.0f, 1 },   // CROUCH_PAIN
    { 173, 177,  5.0f, 1 },   // CROUCH_DEATH
    { 178, 183,  7.0f, 0 },   // DEATH_FALLBACK
    { 184, 189,  7.0f, 0 },   // DEATH_FALLFORWARD
    { 190, 197,  7.0f, 0 },   // DEATH_FALLBACKSLOW
    {   0, 197,  7.0f, 1 }    // FULL
  };

  void MD2::interpolate( const AnimState* anim ) const
  {
    const Point3* currFrame = &vertices[anim->currFrame * nFrameVerts];
    const Point3* nextFrame = &vertices[anim->nextFrame * nFrameVerts];

    Vertex* vertBuffer = mesh.map( GL_READ_WRITE );

    float t = anim->fps * anim->currTime;

    for( int i = 0; i < nFrameVerts; ++i ) {
      Point3 p = currFrame[i] + t * ( nextFrame[i] - currFrame[i] );

      vertBuffer[i].pos[0] = p.x;
      vertBuffer[i].pos[1] = p.y;
      vertBuffer[i].pos[2] = p.z;
    }

    mesh.unmap();
  }

  void MD2::prebuild( const char* name )
  {
    String sPath = "mdl/" + String( name );
    String modelFile = sPath + "/tris.md2";
    String skinFile = sPath + "/skin.jpg";
    String configFile = sPath + "/config.rc";

    log.println( "Prebuilding MD2 model '%s' {", name );
    log.indent();

    Config config;
    config.load( configFile );

    float scale       = config.get( "scale", 0.042f );
    Vec3 translation  = Vec3( config.get( "translate.x", 0.00f ),
                              config.get( "translate.y", 0.00f ),
                              config.get( "translate.z", 0.00f ) );
    Vec3 jumpTransl   = Vec3( config.get( "jumpTranslate.x", 0.00f ),
                              config.get( "jumpTranslate.y", 0.00f ),
                              config.get( "jumpTranslate.z", 0.00f ) );
    Vec3 weaponTransl = Vec3( config.get( "weaponTranslate.x", 0.00f ),
                              config.get( "weaponTranslate.y", 0.00f ),
                              config.get( "weaponTranslate.z", 0.00f ) );


    FILE* file = fopen( modelFile.cstr(), "rb" );
    if( file == null ) {
      throw Exception( "MD2 file does not exist" );
    }

    MD2Header header;
    fread( &header, 1, sizeof( MD2Header ), file );
    if( header.id != FOURCC( 'I', 'D', 'P', '2' ) || header.version != 8 ) {
      fclose( file );
      throw Exception( "MD2 invalid format" );
    }

    int nFrames     = header.nFrames;
    int nFrameVerts = header.nVerts;

    if( nFrames <= 0 || nFrameVerts <= 0 ) {
      fclose( file );
      throw Exception( "MD2 model loading error" );
    }

    DArray<Point3> vertices( nFrames * nFrameVerts );
    DArray<int>    normals( nFrames * nFrameVerts );
    DArray<int>    glCmds( header.nGlCmds );

    char* buffer = new char[nFrames * header.frameSize];

    fseek( file, header.offFrames, SEEK_SET );
    fread( buffer, 1, nFrames * header.frameSize, file );

    fseek( file, header.offGLCmds, SEEK_SET );
    fread( glCmds, 1, header.nGlCmds * sizeof( int ), file );

    for( int i = 0; i < nFrames; ++i ) {
      MD2Frame* pFrame = reinterpret_cast<MD2Frame*>( &buffer[i * header.frameSize] );
      Point3*   pVerts = &vertices[i * nFrameVerts];
      int*      pNormals = &normals[i * nFrameVerts];

      for( int j = 0; j < nFrameVerts; ++j ) {
        pNormals[j] = pFrame->verts[j].normal;
        pVerts[j] = Point3(
          ( float( pFrame->verts[j].p[1] ) * -pFrame->scale[1] ) - pFrame->translate[1],
          ( float( pFrame->verts[j].p[0] ) *  pFrame->scale[0] ) + pFrame->translate[0],
          ( float( pFrame->verts[j].p[2] ) *  pFrame->scale[2] ) + pFrame->translate[2] );

        pVerts[j] = Point3::ORIGIN + scale * ( pVerts[j] - Point3::ORIGIN );
        pVerts[j] += translation;
        if( i == Anim::JUMP ) {
          pVerts[j] += jumpTransl;
        }
      }
    }
    delete[] buffer;
    fclose( file );

    MeshData meshes[nFrames];

    for( int i = 0; i < nFrames; ++i ) {
      const int* cmd = glCmds;

      compiler.beginMesh();
      compiler.enable( CAP_CW );

      if( nFrames == 1 ) {
        compiler.enable( CAP_UNIQUE );
      }

      compiler.texture( 0, GL_TEXTURE_2D, skinFile );
      compiler.begin( GL_TRIANGLES );

      auto vertex = [&]( const int* cmd ) {
        compiler.texCoord( 0, reinterpret_cast<const float*>( cmd ) );
        compiler.normal( NORMALS[ normals[ cmd[2] ] ] );
        compiler.vertex( vertices[ i * nFrameVerts + cmd[2] ] );
      };

      while( int n = *cmd ) {
        int mode;
        if( n < 0 ) {
          mode = GL_TRIANGLE_FAN;
          n = -n;
        }
        else {
          mode = GL_TRIANGLE_STRIP;
        }
        ++cmd;

        vertex( cmd + 0 );
        vertex( cmd + 3 );
        vertex( cmd + 6 );

        for( int j = 3; j < n; ++j ) {
          if( mode == GL_TRIANGLE_FAN ) {
            vertex( cmd + 0 );
            vertex( cmd + ( j - 1 ) * 3 );
            vertex( cmd + j * 3 );
          }
          else if( j % 2 == 1 ) {
            vertex( cmd + ( j - 1 ) * 3 );
            vertex( cmd + ( j - 2 ) * 3 );
            vertex( cmd + j * 3 );
          }
          else {
            vertex( cmd + ( j - 2 ) * 3 );
            vertex( cmd + ( j - 1 ) * 3 );
            vertex( cmd + j * 3 );
          }
        }

        cmd += n * 3;
      }
      compiler.end();
      compiler.endMesh();

      compiler.getMeshData( &meshes[i] );
    }

    size_t size = 0;

    size += 2 * sizeof( int );
    size += compiler.meshSize();

    if( nFrames > 1 ) {
      size += nFrames* meshes[0].vertices.length() * sizeof( float[3] );
    }

    Buffer outBuffer( size );
    OutputStream os = outBuffer.outputStream();

    os.writeInt( nFrames );
    os.writeInt( meshes[0].vertices.length() );

    meshes[0].write( &os );

    if( nFrames > 1 ) {
      for( int i = 0; i < nFrames; ++i ) {
        foreach( vertex, meshes[i].vertices.citer() ) {
          os.writeFloat( vertex->pos[0] );
          os.writeFloat( vertex->pos[1] );
          os.writeFloat( vertex->pos[2] );
        }
      }
    }

    hard_assert( !os.isAvailable() );
    outBuffer.write( sPath + "/tris.ozcMD2" );

    log.unindent();
    log.println( "}" );
  }

  MD2::MD2( const char* name_ ) : name( name_ ), vertices( null ), isLoaded( false )
  {}

  MD2::~MD2()
  {
    log.print( "Unloading MD2 model '%s' ...", name.cstr() );

    mesh.unload();
    delete[] vertices;

    log.printEnd( " OK" );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void MD2::load()
  {
    log.println( "Loading MD2 model '%s' {", name.cstr() );
    log.indent();

    Buffer buffer;
    buffer.read( "mdl/" + name + "/tris.ozcMD2" );
    InputStream is = buffer.inputStream();

    nFrames     = is.readInt();
    nFrameVerts = is.readInt();

    if( nFrames == 1 ) {
      mesh.load( &is, GL_STATIC_DRAW );
    }
    else {
      mesh.load( &is, GL_STREAM_DRAW );

      vertices = new Point3[nFrames * nFrameVerts];

      for( int i = 0; i < nFrames * nFrameVerts; ++i ) {
        vertices[i] = is.readPoint3();
      }
    }

    isLoaded = true;

    log.unindent();
    log.println( "}" );
  }

  void MD2::advance( AnimState* anim, float dt ) const
  {
    anim->currTime += dt;

    while( anim->currTime > anim->frameTime ) {
      anim->currTime -= anim->frameTime;
      anim->currFrame = anim->nextFrame;
      ++anim->nextFrame;

      if( anim->nextFrame > anim->endFrame ) {
        anim->nextFrame = anim->repeat ? anim->startFrame : anim->endFrame;
      }
    }
  }

  void MD2::drawFrame( int ) const
  {
    mesh.drawSolid();
  }

  void MD2::draw( const AnimState* anim ) const
  {
    interpolate( anim );
    mesh.drawSolid();
  }

}
}
