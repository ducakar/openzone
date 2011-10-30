/*
 *  MD2.cpp
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/MD2.hpp"

#include "client/Context.hpp"
#include "client/Colours.hpp"
#include "client/Compiler.hpp"

#include "client/OpenGL.hpp"

#include <sys/stat.h>

namespace oz
{
namespace client
{

const MD2::AnimInfo MD2::ANIM_LIST[] =
{
  // first, last, repeat, fps
  {   0,  39, 1,  9.0f },   // STAND
  {  40,  45, 1, 10.0f },   // RUN
  {  46,  53, 1, 16.0f },   // ATTACK
  {  54,  57, 1,  7.0f },   // PAIN_A
  {  58,  61, 1,  7.0f },   // PAIN_B
  {  62,  65, 1,  7.0f },   // PAIN_C
  {  67,  67, 0,  9.0f },   // JUMP
  {  72,  83, 1,  7.0f },   // FLIP
  {  84,  94, 1,  7.0f },   // SALUTE
  {  95, 111, 1, 10.0f },   // FALLBACK
  { 112, 122, 1,  7.0f },   // WAVE
  { 123, 134, 1,  6.0f },   // POINT
  { 135, 153, 1, 10.0f },   // CROUCH_STAND
  { 154, 159, 1,  7.0f },   // CROUCH_WALK
  { 160, 168, 1, 18.0f },   // CROUCH_ATTACK
  { 169, 172, 1,  7.0f },   // CROUCH_PAIN
  { 173, 177, 1,  5.0f },   // CROUCH_DEATH
  { 178, 183, 0,  7.0f },   // DEATH_FALLBACK
  { 184, 189, 0,  7.0f },   // DEATH_FALLFORWARD
  { 190, 197, 0,  7.0f },   // DEATH_FALLBACKSLOW
  {   0, 197, 1,  7.0f }    // FULL
};

#ifndef OZ_TOOLS

Vertex MD2::animBuffer[MAX_VERTS];

MD2::MD2( int id_ ) : id( id_ ), vertices( null ), positions( null ), normals( 0 ),
    isLoaded( false )
{}

MD2::~MD2()
{
  const String& name = library.models[id].name;

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

  OZ_GL_CHECK_ERROR();
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
      int j = int( vertices[i].pos[0] * float( nFramePositions - 1 ) + 0.5f );

      Vec4 pos    = framePositions[j];
      Vec4 normal = frameNormals[j];

      animBuffer[i].pos[0] = pos.x;
      animBuffer[i].pos[1] = pos.y;
      animBuffer[i].pos[2] = pos.z;

      animBuffer[i].texCoord[0] = vertices[i].texCoord[0];
      animBuffer[i].texCoord[1] = vertices[i].texCoord[1];

      animBuffer[i].normal[0] = normal.x;
      animBuffer[i].normal[1] = normal.y;
      animBuffer[i].normal[2] = normal.z;
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

      animBuffer[i].pos[0] = pos.x;
      animBuffer[i].pos[1] = pos.y;
      animBuffer[i].pos[2] = pos.z;

      animBuffer[i].texCoord[0] = vertices[i].texCoord[0];
      animBuffer[i].texCoord[1] = vertices[i].texCoord[1];

      animBuffer[i].normal[0] = normal.x;
      animBuffer[i].normal[1] = normal.y;
      animBuffer[i].normal[2] = normal.z;
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

void MD2::load()
{
  const String& name = library.models[id].name;
  const String& path = library.models[id].path;

  log.print( "Loading MD2 model '%s' ...", name.cstr() );

  Buffer buffer;
  if( !buffer.read( path ) ) {
    throw Exception( "MD2 cannot read model file" );
  }

  InputStream is  = buffer.inputStream();

  shaderId        = library.shaderIndex( is.readString() );

  nFrames         = is.readInt();
  nFrameVertices  = is.readInt();
  nFramePositions = is.readInt();
  weaponTransf    = is.readMat44();

  if( shader.hasVertexTexture ) {
    uint pbos[2];

    glGenBuffers( 2, pbos );

    glBindBuffer( GL_PIXEL_UNPACK_BUFFER, pbos[0] );
    glBufferData( GL_PIXEL_UNPACK_BUFFER, nFrames * nFramePositions * int( sizeof( Vec4 ) ), 0,
                  GL_STREAM_DRAW );

    positions = reinterpret_cast<Vec4*>( glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY ) );

    for( int i = 0; i < nFramePositions * nFrames; ++i ) {
      positions[i].x = is.readFloat();
      positions[i].y = is.readFloat();
      positions[i].z = is.readFloat();
      positions[i].w = 1.0f;
    }

    glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );

    glGenTextures( 1, &vertexTexId );
    glBindTexture( GL_TEXTURE_2D, vertexTexId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, nFramePositions, nFrames, 0,
                  GL_RGBA, GL_FLOAT, 0 );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glBindBuffer( GL_PIXEL_UNPACK_BUFFER, pbos[0] );
    glBufferData( GL_PIXEL_UNPACK_BUFFER, nFrames * nFramePositions * int( sizeof( Vec4 ) ), 0,
                  GL_STREAM_DRAW );

    normals = reinterpret_cast<Vec4*>( glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY ) );

    for( int i = 0; i < nFramePositions * nFrames; ++i ) {
      normals[i].x  = is.readFloat();
      normals[i].y  = is.readFloat();
      normals[i].z  = is.readFloat();
      normals[i].w  = 0.0f;
    }

    glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );

    glGenTextures( 1, &normalTexId );
    glBindTexture( GL_TEXTURE_2D, normalTexId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, nFramePositions, nFrames, 0,
                  GL_RGBA, GL_FLOAT, 0 );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );

    mesh.load( &is, GL_STATIC_DRAW );

    glDeleteBuffers( 2, pbos );
  }
  else {
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

    const char* meshStart = is.getPos();

    is.readInt();
    is.readInt();

    vertices = new Vertex[nFrameVertices];
    for( int i = 0; i < nFrameVertices; ++i ) {
      vertices[i].read( &is );
    }

    is.setPos( meshStart );

    mesh.load( &is, GL_STREAM_DRAW );
  }

  OZ_GL_CHECK_ERROR();

  isLoaded = true;

  log.printEnd( " OK" );
}

#else // OZ_TOOLS

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

void MD2::prebuild( const char* path )
{
  String sPath = path;
  String modelFile = sPath + "/tris.md2";
  String skinFile = sPath + "/skin.png";
  String configFile = sPath + "/config.rc";

  log.println( "Prebuilding MD2 model '%s' {", path );
  log.indent();

  Config config;
  config.load( configFile );

  FILE* file = fopen( modelFile.cstr(), "rb" );
  if( file == null ) {
    throw Exception( "MD2 file does not exist" );
  }

  MD2Header header;
  fread( &header, 1, sizeof( MD2Header ), file );
  if( header.id[0] != 'I' || header.id[1] != 'D' || header.id[2] != 'P' || header.id[3] != '2' ||
      header.version != 8 )
  {
    fclose( file );
    throw Exception( "MD2 invalid format" );
  }

  if( header.nFrames <= 0 || header.nFramePositions <= 0 ) {
    fclose( file );
    throw Exception( "MD2 model loading error" );
  }

  if( config.get( "forceStatic", false ) ) {
    header.nFrames = 1;
  }

  String shaderName   = config.get( "shader", header.nFrames == 1 ? "mesh" : "md2" );
  float  specular     = config.get( "specular", 0.0f );
  float  scale        = config.get( "scale", 0.04f );

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

  Mat44 weaponTransf = Mat44::translation( weaponTransl );
  weaponTransf.rotateY( Math::rad( weaponRot.y ) );
  weaponTransf.rotateX( Math::rad( weaponRot.x ) );
  weaponTransf.rotateZ( Math::rad( weaponRot.z ) );

  DArray<MD2TexCoord> texCoords( header.nTexCoords );
  DArray<MD2Triangle> triangles( header.nTriangles );
  DArray<Vec3>        normals( header.nFrames * header.nFramePositions );
  DArray<Point3>      vertices( header.nFrames * header.nFramePositions );

  char* frameData = new char[header.nFrames * header.frameSize];

  fseek( file, header.offFrames, SEEK_SET );
  fread( frameData, 1, size_t( header.nFrames * header.frameSize ), file );

  for( int i = 0; i < header.nFrames; ++i ) {
    MD2Frame& frame = *reinterpret_cast<MD2Frame*>( &frameData[i * header.frameSize] );

    for( int j = 0; j < header.nFramePositions; ++j ) {
      Vec3&   normal = normals[i * header.nFramePositions + j];
      Point3& vertex = vertices[i * header.nFramePositions + j];

      normal   = NORMALS[ frame.verts[j].normal ];

      vertex.x = float( frame.verts[j].p[1] ) * -frame.scale[1] - frame.translate[1];
      vertex.y = float( frame.verts[j].p[0] ) *  frame.scale[0] + frame.translate[0];
      vertex.z = float( frame.verts[j].p[2] ) *  frame.scale[2] + frame.translate[2];

      vertex.x = vertex.x * scale + translation.x;
      vertex.y = vertex.y * scale + translation.y;
      vertex.z = vertex.z * scale + translation.z;

      if( ANIM_LIST[Anim::JUMP].firstFrame <= i && i <= ANIM_LIST[Anim::JUMP].lastFrame ) {
        vertex += jumpTransl;
      }
    }
  }

  delete[] frameData;

  fseek( file, header.offTexCoords, SEEK_SET );
  fread( texCoords, 1, size_t( header.nTexCoords ) * sizeof( MD2TexCoord ), file );

  fseek( file, header.offTriangles, SEEK_SET );
  fread( triangles, 1, size_t( header.nTriangles ) * sizeof( MD2Triangle ), file );

  fclose( file );

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
                         float( header.skinHeight - texCoord.t ) / float( header.skinHeight ) );

      if( header.nFrames == 1 ) {
        compiler.normal( normals[ triangles[i].vertices[j] ] );
        compiler.vertex( vertices[ triangles[i].vertices[j] ] );
      }
      else {
        // vertex index in vertex animation buffer
        compiler.animVertex( triangles[i].vertices[j] );
      }
    }
  }

  compiler.end();
  compiler.endMesh();

  triangles.dealloc();
  texCoords.dealloc();

  MeshData mesh;
  compiler.getMeshData( &mesh );

  int nFrameVertices = mesh.vertices.length();

  library.shaderIndex( shaderName );

  if( nFrameVertices > MAX_VERTS ) {
    throw Exception( "MD2 model has too many vertices" );
  }

  Buffer buffer( 4 * 1024 * 1024 );
  OutputStream os = buffer.outputStream();

  os.writeString( shaderName );

  // generate vertex data for animated MD2s
  if( header.nFrames != 1 ) {
    os.writeInt( header.nFrames );
    os.writeInt( nFrameVertices );
    os.writeInt( header.nFramePositions );
    os.writeMat44( weaponTransf );

    // write vertex positions for all frames
    for( int i = 0; i < header.nFrames; ++i ) {
      for( int j = 0; j < header.nFramePositions; ++j ) {
        os.writePoint3( vertices[i * header.nFramePositions + j] );
      }
    }
    // write vertex normals for all frames
    for( int i = 0; i < header.nFrames; ++i ) {
      for( int j = 0; j < header.nFramePositions; ++j ) {
        os.writeVec3( normals[i * header.nFramePositions + j] );
      }
    }

    // if we have an animated model, we use vertex position to save texture coordinate for vertex
    // texture to fetch the positions in both frames and interpolate them in vertex shader
    for( auto vertex : mesh.vertices.iter() ) {
      vertex->pos[0] = ( vertex->pos[0] + 0.5f ) / float( header.nFramePositions );
      vertex->pos[1] = 0.0f;
      vertex->pos[2] = 0.0f;
    }
  }

  normals.dealloc();
  vertices.dealloc();

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

  log.unindent();
  log.println( "}" );
}

#endif // OZ_TOOLS

}
}
