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

}
}
