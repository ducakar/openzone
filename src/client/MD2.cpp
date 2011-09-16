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

  Vertex MD2::animBuffer[MAX_VERTS];

#ifdef OZ_BUILD_TOOLS
  const Vec3 MD2::NORMALS[] =
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
#endif

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
    {  72,  83,  7.0f, 1 },   // FLIP
    {  84,  94,  7.0f, 1 },   // SALUTE
    {  95, 111, 10.0f, 1 },   // FALLBACK
    { 112, 122,  7.0f, 1 },   // WAVE
    { 123, 134,  6.0f, 1 },   // POINT
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

#ifdef OZ_BUILD_TOOLS
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

    if( header.nFrames <= 0 || header.nFramePositions <= 0 ) {
      fclose( file );
      throw Exception( "MD2 model loading error" );
    }

    if( doForceStatic ) {
      header.nFrames = 1;
    }

    DArray<MD2TexCoord> texCoords( header.nTexCoords );
    DArray<MD2Triangle> triangles( header.nTriangles );

    char* frameData = new char[header.nFrames * header.frameSize];

    fseek( file, header.offFrames, SEEK_SET );
    fread( frameData, 1, size_t( header.nFrames * header.frameSize ), file );

    fseek( file, header.offTexCoords, SEEK_SET );
    fread( texCoords, 1, size_t( header.nTexCoords ) * sizeof( MD2TexCoord ), file );

    fseek( file, header.offTriangles, SEEK_SET );
    fread( triangles, 1, size_t( header.nTriangles ) * sizeof( MD2Triangle ), file );

    fclose( file );

    String shaderName   = config.get( "shader", header.nFrames == 1 ? "mesh" : "md2" );
    float  scale        = config.get( "scale", 0.04f );
    float  specular     = config.get( "specular", 0.0f );

    Vec3   translation  = Vec3( config.get( "translate.x", +0.00f ),
                                 config.get( "translate.y", +0.00f ),
                                 config.get( "translate.z", -0.04f ) );
    Vec3   jumpTransl   = Vec3( config.get( "jumpTranslate.x", 0.00f ),
                                 config.get( "jumpTranslate.y", 0.00f ),
                                 config.get( "jumpTranslate.z", 0.00f ) );
    Vec3   weaponTransl = Vec3( config.get( "weaponTranslate.x", 0.00f ),
                                 config.get( "weaponTranslate.y", 0.00f ),
                                 config.get( "weaponTranslate.z", 0.00f ) );
    Vec3   weaponRot    = Vec3( config.get( "weaponRotate.x", 0.00f ),
                                 config.get( "weaponRotate.y", 0.00f ),
                                 config.get( "weaponRotate.z", 0.00f ) );
    Mat44  weaponTransf = Mat44::ID;

    weaponTransf.rotateX( Math::rad( weaponRot.x ) );
    weaponTransf.rotateY( Math::rad( weaponRot.y ) );
    weaponTransf.rotateZ( Math::rad( weaponRot.z ) );
    weaponTransf.translate( weaponTransl );

    header.nFrames = doForceStatic ? 1 : header.nFrames;

    compiler.beginMesh();
    compiler.enable( CAP_UNIQUE );
    compiler.enable( CAP_CW );
    compiler.material( GL_SPECULAR, specular );
    compiler.texture( skinFile );

    compiler.begin( GL_TRIANGLES );

    for( int i = 0; i < header.nTriangles; ++i ) {
      for( int j = 0; j < 3; ++j ) {
        const MD2TexCoord& texCoord = texCoords[ triangles[i].texCoords[j] ];

        compiler.texCoord( float( texCoord.s ) / float( header.skinWidth ),
                           float( texCoord.t ) / float( header.skinHeight ) );
        // position index (to make it unique and) to replace it later by the actual coordinates
        compiler.vertex( float( triangles[i].vertices[j] ), 0.0f, 0.0f );
      }
    }

    compiler.end();
    compiler.endMesh();

    MeshData mesh;
    compiler.getMeshData( &mesh );

    int nFrameVertices = mesh.vertices.length();

    translator.shaderIndex( shaderName );

    if( nFrameVertices > MAX_VERTS ) {
      delete[] frameData;

      throw Exception( "MD2 model has too many vertices" );
    }

    OutputStream os = buffer.outputStream();

    os.writeString( shaderName );

    // generate vertex data for animated MD2s
    if( header.nFrames == 1 ) {
      // replace position indices by their actual coordinates if we have a static mesh
      MD2Frame& frame = *reinterpret_cast<MD2Frame*>( &frameData[0] );

      foreach( vertex, mesh.vertices.iter() ) {
        int index = int( vertex->pos.x + 0.5f );

        vertex->pos.x = float( frame.verts[index].p[1] ) * -frame.scale[1] - frame.translate[1];
        vertex->pos.y = float( frame.verts[index].p[0] ) *  frame.scale[0] + frame.translate[0];
        vertex->pos.z = float( frame.verts[index].p[2] ) *  frame.scale[2] + frame.translate[2];

        vertex->pos.x = vertex->pos.x * scale + translation.x;
        vertex->pos.y = vertex->pos.y * scale + translation.y;
        vertex->pos.z = vertex->pos.z * scale + translation.z;

        vertex->normal.x = NORMALS[ frame.verts[index].normal ].x;
        vertex->normal.y = NORMALS[ frame.verts[index].normal ].y;
        vertex->normal.z = NORMALS[ frame.verts[index].normal ].z;
      }
    }
    else {
      os.writeInt( header.nFrames );
      os.writeInt( nFrameVertices );
      os.writeInt( header.nFramePositions );
      os.writeMat44( weaponTransf );

      // write vertex positions for all frames
      for( int i = 0; i < header.nFrames; ++i ) {
        MD2Frame& frame = *reinterpret_cast<MD2Frame*>( &frameData[i * header.frameSize] );

        for( int j = 0; j < header.nFramePositions; ++j ) {
          Point3 p;

          p.x = float( frame.verts[j].p[1] ) * -frame.scale[1] - frame.translate[1];
          p.y = float( frame.verts[j].p[0] ) *  frame.scale[0] + frame.translate[0];
          p.z = float( frame.verts[j].p[2] ) *  frame.scale[2] + frame.translate[2];

          p.x = p.x * scale + translation.x;
          p.y = p.y * scale + translation.y;
          p.z = p.z * scale + translation.z;

          if( ANIM_LIST[Anim::JUMP].firstFrame <= i && i <= ANIM_LIST[Anim::JUMP].lastFrame ) {
            p += jumpTransl;
          }

          os.writePoint3( p );
        }
      }
      // write vertex normals for all frames
      for( int i = 0; i < header.nFrames; ++i ) {
        MD2Frame& frame = *reinterpret_cast<MD2Frame*>( &frameData[i * header.frameSize] );

        for( int j = 0; j < header.nFramePositions; ++j ) {
          os.writeVec3( NORMALS[ frame.verts[j].normal ] );
        }
      }

      // if we have an animated model, we use vertex position to save texture coordinate for vertex
      // texture to fetch the positions in both frames and interpolate them in vertex shader
      foreach( vertex, mesh.vertices.iter() ) {
        vertex->pos.x = ( vertex->pos.x + 0.5f ) / float( header.nFramePositions );
        vertex->pos.y = 0.0f;
        vertex->pos.z = 0.0f;
      }
    }

    mesh.write( &os );

    if( header.nFrames != 1 ) {
      log.print( "Writing to '%s' ...", ( sPath + ".ozcMD2" ).cstr() );
      buffer.write( sPath + ".ozcMD2", os.length() );
      log.printEnd( " OK" );
    }
    else {
      log.print( "Writing to '%s' ...", ( sPath + ".ozcSMM" ).cstr() );
      buffer.write( sPath + ".ozcSMM", os.length() );
      log.printEnd( " OK" );
    }

    delete[] frameData;

    log.unindent();
    log.println( "}" );
  }
#endif

  MD2::MD2( int id_ ) : id( id_ ), vertices( null ), positions( null ), normals( 0 ),
      isLoaded( false )
  {}

  MD2::~MD2()
  {
    const String& name = translator.models[id].name;

    log.print( "Unloading MD2 model '%s' ...", name.cstr() );

    if( shader.hasVertexTexture ) {
      glDeleteTextures( 1, &vertexTexId );
      glDeleteTextures( 1, &normalTexId );
    }
    else {
      delete[] vertices;
      delete[] positions;
      delete[] normals;
    }

    mesh.unload();

    log.printEnd( " OK" );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void MD2::load()
  {
    const String& name = translator.models[id].name;
    const String& path = translator.models[id].path;

    log.print( "Loading MD2 model '%s' ...", name.cstr() );

    if( !buffer.read( path ) ) {
      throw Exception( "MD2 cannot read model file" );
    }

    InputStream is  = buffer.inputStream();

    shaderId        = translator.shaderIndex( is.readString() );

    nFrames         = is.readInt();
    nFrameVertices  = is.readInt();
    nFramePositions = is.readInt();
    weaponTransf    = is.readMat44();

    positions = new Vec4[nFramePositions * nFrames];
    normals   = new Vec4[nFramePositions * nFrames];

    for( int i = 0; i < nFramePositions * nFrames; ++i ) {
      positions[i].x = is.readFloat();
      positions[i].y = is.readFloat();
      positions[i].z = is.readFloat();
      positions[i].w = 1.0f;
    }

    for( int i = 0; i < nFramePositions * nFrames; ++i ) {
      normals[i].x  = is.readFloat();
      normals[i].y  = is.readFloat();
      normals[i].z  = is.readFloat();
      normals[i].w  = 0.0f;
    }

    if( shader.hasVertexTexture ) {
      glGenTextures( 1, &vertexTexId );
      glBindTexture( GL_TEXTURE_2D, vertexTexId );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, nFramePositions, nFrames, 0,
                    GL_RGBA, GL_FLOAT, positions );
      glBindTexture( GL_TEXTURE_2D, 0 );

      glGenTextures( 1, &normalTexId );
      glBindTexture( GL_TEXTURE_2D, normalTexId );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, nFramePositions, nFrames, 0,
                    GL_RGBA, GL_FLOAT, normals );
      glBindTexture( GL_TEXTURE_2D, 0 );

      delete[] positions;
      delete[] normals;

      mesh.load( &is, GL_STATIC_DRAW );
    }

    if( !shader.hasVertexTexture ) {
      const Vertex* vertexBuffer = reinterpret_cast<const Vertex*>( is.getPos() + 2*sizeof( int ) );

      vertices = new Vertex[nFrameVertices];
      for( int i = 0; i < nFrameVertices; ++i ) {
        vertices[i] = vertexBuffer[i];
      }

      mesh.load( &is, GL_STREAM_DRAW );
    }

    hard_assert( glGetError() == GL_NO_ERROR );

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
    shader.use( shaderId );

    if( shader.hasVertexTexture ) {
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, vertexTexId );
      glActiveTexture( GL_TEXTURE2 );
      glBindTexture( GL_TEXTURE_2D, normalTexId );
      glActiveTexture( GL_TEXTURE0 );
    }
    else {
      const Vec4* framePositions = &positions[frame * nFramePositions];
      const Vec4* frameNormals   = &normals[frame * nFramePositions];

      for( int i = 0; i < nFrameVertices; ++i ) {
        int j = int( vertices[i].pos.x * float( nFramePositions - 1 ) + 0.5f );

        Vec4 pos    = framePositions[j];
        Vec4 normal = frameNormals[j];

        animBuffer[i].pos      = Point3( pos );
        animBuffer[i].texCoord = vertices[i].texCoord;
        animBuffer[i].normal   = normal;
      }

      mesh.upload( animBuffer, nFrameVertices, GL_STREAM_DRAW );
    }

    glUniform3f( param.oz_MD2Anim, float( frame ) / float( nFrames ), 0.0f, 0.0f );
    tf.apply();

    mesh.draw( Mesh::SOLID_BIT );
  }

  void MD2::draw( const AnimState* anim ) const
  {
    shader.use( shaderId );

    if( shader.hasVertexTexture ) {
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, vertexTexId );
      glActiveTexture( GL_TEXTURE2 );
      glBindTexture( GL_TEXTURE_2D, normalTexId );
      glActiveTexture( GL_TEXTURE0 );
    }
    else {
      const Vec4* currFramePositions = &positions[anim->currFrame * nFramePositions];
      const Vec4* nextFramePositions = &positions[anim->nextFrame * nFramePositions];
      const Vec4* currFrameNormals   = &normals[anim->currFrame * nFramePositions];
      const Vec4* nextFrameNormals   = &normals[anim->nextFrame * nFramePositions];

      float t = anim->fps * anim->currTime;

      for( int i = 0; i < nFrameVertices; ++i ) {
        int j = int( vertices[i].pos[0] * float( nFramePositions - 1 ) + 0.5f );

        Vec4 pos    = currFramePositions[j] + t * ( nextFramePositions[j] - currFramePositions[j] );
        Vec4 normal = currFrameNormals[j]   + t * ( nextFrameNormals[j]   - currFrameNormals[j]   );

        animBuffer[i].pos      = Point3( pos );
        animBuffer[i].texCoord = vertices[i].texCoord;
        animBuffer[i].normal   = normal;
      }

      mesh.upload( animBuffer, nFrameVertices, GL_STREAM_DRAW );
    }

    glUniform3f( param.oz_MD2Anim,
                 float( anim->currFrame ) / float( nFrames ),
                 float( anim->nextFrame ) / float( nFrames ),
                 anim->fps * anim->currTime );
    tf.apply();

    mesh.draw( Mesh::SOLID_BIT );
  }

}
}
