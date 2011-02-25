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
    int nFrameVerts;
    int nTexCoords;
    int nTriangles;
    int nGlCmds;
    int nFrames;

    int offSkins;
    int offTexCoords;
    int offTriangles;
    int offFrames;
    int offGLCmds;
    int offEnd;
  };

  struct MD2Vertex
  {
    ubyte p[3];
    ubyte normal;
  };

  struct MD2TexCoord
  {
    short s;
    short t;
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

  const MD2::AnimInfo MD2::ANIM_LIST[] =
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

  void MD2::setFrame( int frame ) const
  {
    const Point3* currFrame = &vertices[frame * nFrameVerts];

    for( int i = 0; i < nFrameVerts; ++i ) {
      frameVerts[i].pos[0] = currFrame[i].x;
      frameVerts[i].pos[1] = currFrame[i].y;
      frameVerts[i].pos[2] = currFrame[i].z;
    }

    mesh.upload( frameVerts, nFrameVerts, GL_STATIC_DRAW );
  }

  void MD2::interpolate( const AnimState* anim ) const
  {
    const Point3* currFrame = &vertices[anim->currFrame * nFrameVerts];
    const Point3* nextFrame = &vertices[anim->nextFrame * nFrameVerts];

    float t = anim->fps * anim->currTime;

    for( int i = 0; i < nFrameVerts; ++i ) {
      Point3 pos = currFrame[i] + t * ( nextFrame[i] - currFrame[i] );

      frameVerts[i].pos[0] = pos.x;
      frameVerts[i].pos[1] = pos.y;
      frameVerts[i].pos[2] = pos.z;
    }

    mesh.upload( frameVerts, nFrameVerts, GL_STREAM_DRAW );
  }

  void MD2::prebuild( const char* path )
  {
    String sPath = path;
    String modelFile = sPath + "/tris.md2";
    String skinFile = sPath + "/skin.jpg";
    String configFile = sPath + "/config.rc";

    log.println( "Prebuilding MD2 model '%s' {", path );
    log.indent();

    Config config;
    config.load( configFile );

    bool doForceStatic = config.get( "forceStatic", false );
    float scale        = config.get( "scale", 0.042f );
    Vec3 translation   = Vec3( config.get( "translate.x", 0.00f ),
                               config.get( "translate.y", 0.00f ),
                               config.get( "translate.z", 0.00f ) );
    Vec3 jumpTransl    = Vec3( config.get( "jumpTranslate.x", 0.00f ),
                               config.get( "jumpTranslate.y", 0.00f ),
                               config.get( "jumpTranslate.z", 0.00f ) );
    Vec3 weaponTransl  = Vec3( config.get( "weaponTranslate.x", 0.00f ),
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

    if( header.nFrames <= 0 || header.nFrameVerts <= 0 ) {
      fclose( file );
      throw Exception( "MD2 model loading error" );
    }

    DArray<MD2TexCoord> texCoords( header.nTexCoords );
    DArray<MD2Triangle> triangles( header.nTriangles );

    char* buffer = new char[header.nFrames * header.frameSize];

    fseek( file, header.offFrames, SEEK_SET );
    fread( buffer, 1, header.nFrames * header.frameSize, file );

    fseek( file, header.offTexCoords, SEEK_SET );
    fread( texCoords, 1, header.nTexCoords * sizeof( MD2TexCoord ), file );

    fseek( file, header.offTriangles, SEEK_SET );
    fread( triangles, 1, header.nTriangles * sizeof( MD2Triangle ), file );

    fclose( file );

    header.nFrames = doForceStatic ? 1 : header.nFrames;

    DArray<Point3> rawVertices( header.nFrames * header.nFrameVerts );
    Point3* currVertex = rawVertices;

    // transform vertices
    for( int i = 0; i < header.nFrames; ++i ) {
      MD2Frame& frame = *reinterpret_cast<MD2Frame*>( &buffer[i * header.frameSize] );

      for( int j = 0; j < header.nFrameVerts; ++j ) {
        currVertex->x = float( frame.verts[j].p[1] ) * -frame.scale[1] - frame.translate[1];
        currVertex->y = float( frame.verts[j].p[0] ) * frame.scale[0] + frame.translate[0];
        currVertex->z = float( frame.verts[j].p[2] ) * frame.scale[2] + frame.translate[2];

        *currVertex = Point3::ORIGIN + ( *currVertex - Point3::ORIGIN ) * scale + translation;

        if( ANIM_LIST[Anim::JUMP].firstFrame <= i && i <= ANIM_LIST[Anim::JUMP].lastFrame ) {
          *currVertex += jumpTransl;
        }

        ++currVertex;
      }
    }
    hard_assert( currVertex == rawVertices + rawVertices.length() );

    MD2Frame& frame = *reinterpret_cast<MD2Frame*>( buffer );

    compiler.beginMesh();
    compiler.enable( CAP_UNIQUE );
    compiler.enable( CAP_CW );
    compiler.texture( 0, skinFile );

    compiler.begin( GL_TRIANGLES );

    for( int i = 0; i < header.nTriangles; ++i ) {
      for( int j = 0; j < 3; ++j ) {
        const MD2TexCoord& texCoord = texCoords[ triangles[i].texCoords[j] ];

        compiler.texCoord( float( texCoord.s ) / float( header.skinWidth ),
                           float( texCoord.t ) / float( header.skinHeight ) );
        // vertex index (to make it unique and) to replace it later by the actual coordinates
        compiler.vertex( float( triangles[i].vertices[j] ), 0.0f, 0.0f );
      }
    }

    compiler.end();
    compiler.endMesh();

    MeshData mesh;
    compiler.getMeshData( &mesh );

    int nMeshVerts = mesh.vertices.length();
    DArray<Point3> vertices;

    // generate vertex data for animated MD2s
    if( header.nFrames > 1 ) {
      vertices.alloc( header.nFrames * nMeshVerts );
      Point3* currVertex = vertices;

      for( int i = 0; i < header.nFrames; ++i ) {
        foreach( vertex, mesh.vertices.citer() ) {
          int index = int( vertex->pos[0] + 0.5f );

          *currVertex = rawVertices[i * header.nFrameVerts + index];
          ++currVertex;
        }
      }
      hard_assert( currVertex == vertices + vertices.length() );
    }

    // replace vertices by actual coordinates
    foreach( vertex, mesh.vertices.iter() ) {
      int index = int( vertex->pos[0] + 0.5f );

      vertex->pos[0] = rawVertices[( header.nFrames - 1 ) * header.nFrameVerts + index].x;
      vertex->pos[1] = rawVertices[( header.nFrames - 1 ) * header.nFrameVerts + index].y;
      vertex->pos[2] = rawVertices[( header.nFrames - 1 ) * header.nFrameVerts + index].z;

      vertex->normal[0] = NORMALS[ frame.verts[index].normal ].x;
      vertex->normal[1] = NORMALS[ frame.verts[index].normal ].y;
      vertex->normal[2] = NORMALS[ frame.verts[index].normal ].z;
    }

    delete[] buffer;

    size_t size = 0;

    size += mesh.getSize();

    if( header.nFrames != 1 ) {
      size += 2 * sizeof( int );
      size += sizeof( float[3] );
      size += nMeshVerts * sizeof( Vertex );
      size += header.nFrames * nMeshVerts * sizeof( float[3] );
    }

    Buffer outBuffer( static_cast<int>( size ) );
    OutputStream os = outBuffer.outputStream();

    if( header.nFrames != 1 ) {
      os.writeInt( header.nFrames );
      os.writeInt( nMeshVerts );
      os.writeVec3( weaponTransl );
    }

    mesh.write( &os );

    if( header.nFrames != 1 ) {
      foreach( vertex, mesh.vertices.citer() ) {
        vertex->write( &os );
      }
      foreach( vertex, vertices.citer() ) {
        os.writePoint3( *vertex );
      }
    }

    hard_assert( !os.isAvailable() );

    if( header.nFrames != 1 ) {
      log.print( "Writing to '%s' ...", ( sPath + ".ozcMD2" ).cstr() );
      outBuffer.write( sPath + ".ozcMD2" );
      log.printEnd( " OK" );
    }
    else {
      log.print( "Writing to '%s' ...", ( sPath + ".ozcSMM" ).cstr() );
      outBuffer.write( sPath + ".ozcSMM" );
      log.printEnd( " OK" );
    }

    log.unindent();
    log.println( "}" );
  }

  MD2::MD2( int id_ ) : id( id_ ), frameVerts( null ), vertices( null ),
      isLoaded( false )
  {}

  MD2::~MD2()
  {
    const String& name = translator.models[id].name;

    log.print( "Unloading MD2 model '%s' ...", name.cstr() );

    mesh.unload();
    delete[] frameVerts;
    delete[] vertices;

    log.printEnd( " OK" );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void MD2::load()
  {
    const String& name = translator.models[id].name;
    const String& path = translator.models[id].path;

    log.print( "Loading MD2 model '%s' ...", name.cstr() );

    Buffer buffer;
    if( !buffer.read( path ) ) {
      throw Exception( "MD2 cannot read model file" );
    }
    InputStream is = buffer.inputStream();

    nFrames      = is.readInt();
    nFrameVerts  = is.readInt();
    weaponTransl = is.readVec3();

    if( nFrames == 1 ) {
      mesh.load( &is, GL_STATIC_DRAW );
    }
    else {
      mesh.load( &is, GL_STREAM_DRAW );

      frameVerts = new Vertex[nFrameVerts];
      vertices = new Point3[nFrames * nFrameVerts];

      for( int i = 0; i < nFrameVerts; ++i ) {
        frameVerts[i].read( &is );
      }
      for( int i = 0; i < nFrames * nFrameVerts; ++i ) {
        vertices[i] = is.readPoint3();
      }
    }

    hard_assert( !is.isAvailable() );

    isLoaded = true;

    log.printEnd( " OK" );
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

  void MD2::drawFrame( int frame ) const
  {
    if( nFrames != 1 ) {
      setFrame( frame );
    }
    mesh.draw( Mesh::SOLID_BIT );
  }

  void MD2::draw( const AnimState* anim ) const
  {
    interpolate( anim );
    mesh.draw( Mesh::SOLID_BIT );
  }

}
}
