/*
 *  MD2.cpp
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "MD2.h"

#include "Context.h"

#define MD2_ID                  ( ( '2' << 24 ) | ( 'P' << 16 ) | ( 'D' << 8 ) | 'I' )
#define MD2_VERSION             8

#define MD2_SHADEDOT_QUANT      16
#define MD2_MAX_VERTS           2048

namespace oz
{
namespace client
{

  struct MD2Header
  {
    int id;
    int version;

    int skinwidth;
    int skinheight;
    int framesize;

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
    ubyte v[3];
    ubyte iLightNormal;
  };

  struct MD2TexCoord
  {
    short s;
    short t;
  };

  struct MD2Frame
  {
    Vec3 scale;
    Vec3 translate;
    char name[16];
    MD2Vertex verts[1];
  };

  struct MD2Triangle
  {
    short iVerts[3];
    short iTexCoords[3];
  };

  float MD2::anorms[][3] =
  {
    // normal vectors
    { -0.525731f,  0.000000f,  0.850651f },
    { -0.442863f,  0.238856f,  0.864188f },
    { -0.295242f,  0.000000f,  0.955423f },
    { -0.309017f,  0.500000f,  0.809017f },
    { -0.162460f,  0.262866f,  0.951056f },
    {  0.000000f,  0.000000f,  1.000000f },
    {  0.000000f,  0.850651f,  0.525731f },
    { -0.147621f,  0.716567f,  0.681718f },
    {  0.147621f,  0.716567f,  0.681718f },
    {  0.000000f,  0.525731f,  0.850651f },
    {  0.309017f,  0.500000f,  0.809017f },
    {  0.525731f,  0.000000f,  0.850651f },
    {  0.295242f,  0.000000f,  0.955423f },
    {  0.442863f,  0.238856f,  0.864188f },
    {  0.162460f,  0.262866f,  0.951056f },
    { -0.681718f,  0.147621f,  0.716567f },
    { -0.809017f,  0.309017f,  0.500000f },
    { -0.587785f,  0.425325f,  0.688191f },
    { -0.850651f,  0.525731f,  0.000000f },
    { -0.864188f,  0.442863f,  0.238856f },
    { -0.716567f,  0.681718f,  0.147621f },
    { -0.688191f,  0.587785f,  0.425325f },
    { -0.500000f,  0.809017f,  0.309017f },
    { -0.238856f,  0.864188f,  0.442863f },
    { -0.425325f,  0.688191f,  0.587785f },
    { -0.716567f,  0.681718f, -0.147621f },
    { -0.500000f,  0.809017f, -0.309017f },
    { -0.525731f,  0.850651f,  0.000000f },
    {  0.000000f,  0.850651f, -0.525731f },
    { -0.238856f,  0.864188f, -0.442863f },
    {  0.000000f,  0.955423f, -0.295242f },
    { -0.262866f,  0.951056f, -0.162460f },
    {  0.000000f,  1.000000f,  0.000000f },
    {  0.000000f,  0.955423f,  0.295242f },
    { -0.262866f,  0.951056f,  0.162460f },
    {  0.238856f,  0.864188f,  0.442863f },
    {  0.262866f,  0.951056f,  0.162460f },
    {  0.500000f,  0.809017f,  0.309017f },
    {  0.238856f,  0.864188f, -0.442863f },
    {  0.262866f,  0.951056f, -0.162460f },
    {  0.500000f,  0.809017f, -0.309017f },
    {  0.850651f,  0.525731f,  0.000000f },
    {  0.716567f,  0.681718f,  0.147621f },
    {  0.716567f,  0.681718f, -0.147621f },
    {  0.525731f,  0.850651f,  0.000000f },
    {  0.425325f,  0.688191f,  0.587785f },
    {  0.864188f,  0.442863f,  0.238856f },
    {  0.688191f,  0.587785f,  0.425325f },
    {  0.809017f,  0.309017f,  0.500000f },
    {  0.681718f,  0.147621f,  0.716567f },
    {  0.587785f,  0.425325f,  0.688191f },
    {  0.955423f,  0.295242f,  0.000000f },
    {  1.000000f,  0.000000f,  0.000000f },
    {  0.951056f,  0.162460f,  0.262866f },
    {  0.850651f, -0.525731f,  0.000000f },
    {  0.955423f, -0.295242f,  0.000000f },
    {  0.864188f, -0.442863f,  0.238856f },
    {  0.951056f, -0.162460f,  0.262866f },
    {  0.809017f, -0.309017f,  0.500000f },
    {  0.681718f, -0.147621f,  0.716567f },
    {  0.850651f,  0.000000f,  0.525731f },
    {  0.864188f,  0.442863f, -0.238856f },
    {  0.809017f,  0.309017f, -0.500000f },
    {  0.951056f,  0.162460f, -0.262866f },
    {  0.525731f,  0.000000f, -0.850651f },
    {  0.681718f,  0.147621f, -0.716567f },
    {  0.681718f, -0.147621f, -0.716567f },
    {  0.850651f,  0.000000f, -0.525731f },
    {  0.809017f, -0.309017f, -0.500000f },
    {  0.864188f, -0.442863f, -0.238856f },
    {  0.951056f, -0.162460f, -0.262866f },
    {  0.147621f,  0.716567f, -0.681718f },
    {  0.309017f,  0.500000f, -0.809017f },
    {  0.425325f,  0.688191f, -0.587785f },
    {  0.442863f,  0.238856f, -0.864188f },
    {  0.587785f,  0.425325f, -0.688191f },
    {  0.688191f,  0.587785f, -0.425325f },
    { -0.147621f,  0.716567f, -0.681718f },
    { -0.309017f,  0.500000f, -0.809017f },
    {  0.000000f,  0.525731f, -0.850651f },
    { -0.525731f,  0.000000f, -0.850651f },
    { -0.442863f,  0.238856f, -0.864188f },
    { -0.295242f,  0.000000f, -0.955423f },
    { -0.162460f,  0.262866f, -0.951056f },
    {  0.000000f,  0.000000f, -1.000000f },
    {  0.295242f,  0.000000f, -0.955423f },
    {  0.162460f,  0.262866f, -0.951056f },
    { -0.442863f, -0.238856f, -0.864188f },
    { -0.309017f, -0.500000f, -0.809017f },
    { -0.162460f, -0.262866f, -0.951056f },
    {  0.000000f, -0.850651f, -0.525731f },
    { -0.147621f, -0.716567f, -0.681718f },
    {  0.147621f, -0.716567f, -0.681718f },
    {  0.000000f, -0.525731f, -0.850651f },
    {  0.309017f, -0.500000f, -0.809017f },
    {  0.442863f, -0.238856f, -0.864188f },
    {  0.162460f, -0.262866f, -0.951056f },
    {  0.238856f, -0.864188f, -0.442863f },
    {  0.500000f, -0.809017f, -0.309017f },
    {  0.425325f, -0.688191f, -0.587785f },
    {  0.716567f, -0.681718f, -0.147621f },
    {  0.688191f, -0.587785f, -0.425325f },
    {  0.587785f, -0.425325f, -0.688191f },
    {  0.000000f, -0.955423f, -0.295242f },
    {  0.000000f, -1.000000f,  0.000000f },
    {  0.262866f, -0.951056f, -0.162460f },
    {  0.000000f, -0.850651f,  0.525731f },
    {  0.000000f, -0.955423f,  0.295242f },
    {  0.238856f, -0.864188f,  0.442863f },
    {  0.262866f, -0.951056f,  0.162460f },
    {  0.500000f, -0.809017f,  0.309017f },
    {  0.716567f, -0.681718f,  0.147621f },
    {  0.525731f, -0.850651f,  0.000000f },
    { -0.238856f, -0.864188f, -0.442863f },
    { -0.500000f, -0.809017f, -0.309017f },
    { -0.262866f, -0.951056f, -0.162460f },
    { -0.850651f, -0.525731f,  0.000000f },
    { -0.716567f, -0.681718f, -0.147621f },
    { -0.716567f, -0.681718f,  0.147621f },
    { -0.525731f, -0.850651f,  0.000000f },
    { -0.500000f, -0.809017f,  0.309017f },
    { -0.238856f, -0.864188f,  0.442863f },
    { -0.262866f, -0.951056f,  0.162460f },
    { -0.864188f, -0.442863f,  0.238856f },
    { -0.809017f, -0.309017f,  0.500000f },
    { -0.688191f, -0.587785f,  0.425325f },
    { -0.681718f, -0.147621f,  0.716567f },
    { -0.442863f, -0.238856f,  0.864188f },
    { -0.587785f, -0.425325f,  0.688191f },
    { -0.309017f, -0.500000f,  0.809017f },
    { -0.147621f, -0.716567f,  0.681718f },
    { -0.425325f, -0.688191f,  0.587785f },
    { -0.162460f, -0.262866f,  0.951056f },
    {  0.442863f, -0.238856f,  0.864188f },
    {  0.162460f, -0.262866f,  0.951056f },
    {  0.309017f, -0.500000f,  0.809017f },
    {  0.147621f, -0.716567f,  0.681718f },
    {  0.000000f, -0.525731f,  0.850651f },
    {  0.425325f, -0.688191f,  0.587785f },
    {  0.587785f, -0.425325f,  0.688191f },
    {  0.688191f, -0.587785f,  0.425325f },
    { -0.955423f,  0.295242f,  0.000000f },
    { -0.951056f,  0.162460f,  0.262866f },
    { -1.000000f,  0.000000f,  0.000000f },
    { -0.850651f,  0.000000f,  0.525731f },
    { -0.955423f, -0.295242f,  0.000000f },
    { -0.951056f, -0.162460f,  0.262866f },
    { -0.864188f,  0.442863f, -0.238856f },
    { -0.951056f,  0.162460f, -0.262866f },
    { -0.809017f,  0.309017f, -0.500000f },
    { -0.864188f, -0.442863f, -0.238856f },
    { -0.951056f, -0.162460f, -0.262866f },
    { -0.809017f, -0.309017f, -0.500000f },
    { -0.681718f,  0.147621f, -0.716567f },
    { -0.681718f, -0.147621f, -0.716567f },
    { -0.850651f,  0.000000f, -0.525731f },
    { -0.688191f,  0.587785f, -0.425325f },
    { -0.587785f,  0.425325f, -0.688191f },
    { -0.425325f,  0.688191f, -0.587785f },
    { -0.425325f, -0.688191f, -0.587785f },
    { -0.587785f, -0.425325f, -0.688191f },
    { -0.688191f, -0.587785f, -0.425325f }
  };

  MD2::Anim MD2::animList[] =
  {
    // first, last, fps
    {   0,  39,  9.0f },   // STAND
    {  40,  45, 10.0f },   // RUN
    {  46,  53, 10.0f },   // ATTACK
    {  54,  57,  7.0f },   // PAIN_A
    {  58,  61,  7.0f },   // PAIN_B
    {  62,  65,  7.0f },   // PAIN_C
    {  67,  67,  9.0f },   // JUMP
    {  72,  83,  7.0f },   // FLIP
    {  84,  94,  7.0f },   // SALUTE
    {  95, 111, 10.0f },   // FALLBACK
    { 112, 122,  7.0f },   // WAVE
    { 123, 134,  6.0f },   // POINT
    { 135, 153, 10.0f },   // CROUCH_STAND
    { 154, 159,  7.0f },   // CROUCH_WALK
    { 160, 168, 10.0f },   // CROUCH_ATTACK
    { 196, 172,  7.0f },   // CROUCH_PAIN
    { 173, 177,  5.0f },   // CROUCH_DEATH
    { 178, 183,  7.0f },   // DEATH_FALLBACK
    { 184, 189,  7.0f },   // DEATH_FALLFORWARD
    { 190, 197,  7.0f },   // DEATH_FALLBACKSLOW
    {   0, 197,  7.0f }    // FULL
  };

  MD2::MD2() : nFrames( 0 ), nVerts( 0 ), nGlCmds( 0 ),
      verts( null ), glCmds( null ), lightNormals( null )
  {}

  bool MD2::load( const char *name )
  {
    FILE      *file;
    MD2Header header;
    char      *buffer;
    MD2Frame  *pFrame;
    Vec3      *pVerts;
    int       *pNormals;

    String sPath = String( "mdl/" ) + name;
    String modelFile = sPath + "/tris.md2";
    String skinFile = sPath + "/skin.jpg";
    String configFile = sPath + "/" + String( name ) + ".xml";

    logFile.print( "Loading MD2 model '%s' ... ", modelFile.cstr() );

    file = fopen( modelFile.cstr(), "rb" );
    if( file == null ) {
      logFile.printEnd( "No such file" );

      assert( false );
      return false;
    }

    fread( &header, 1, sizeof( header ), file );
    if( header.id != MD2_ID || header.version != MD2_VERSION ) {
      fclose( file );
      logFile.printEnd( "Invalid file" );

      assert( false );
      return false;
    }

    nFrames = header.nFrames;
    nVerts = header.nVerts;
    nGlCmds = header.nGlCmds;

    verts = new Vec3[nVerts * nFrames];
    glCmds = new int[nGlCmds];
    lightNormals = new int[nVerts * nFrames];
    buffer = new char[nFrames * header.framesize];

    fseek( file, header.offFrames, SEEK_SET );
    fread( buffer, 1, nFrames * header.framesize, file );

    fseek( file, header.offGLCmds, SEEK_SET );
    fread( glCmds, 1, nGlCmds * sizeof( int ), file );

    for( int i = 0; i < nFrames; i++ ) {
      pFrame = (MD2Frame*) &buffer[header.framesize * i];
      pVerts = &verts[nVerts * i];
      pNormals = &lightNormals[nVerts * i];

      for( int j = 0; j < nVerts; j++ ) {
        pVerts[j] = Vec3(
          ( (float) pFrame->verts[j].v[0] * pFrame->scale.x ) + pFrame->translate.x,
          ( (float) pFrame->verts[j].v[1] * pFrame->scale.y ) + pFrame->translate.y,
          ( (float) pFrame->verts[j].v[2] * pFrame->scale.z ) + pFrame->translate.z );

        pNormals[j] = pFrame->verts[j].iLightNormal;
      }
    }
    delete[] buffer;
    fclose( file );

    logFile.printEnd( "OK" );

    texId = context.loadTexture( skinFile, true );

    Config config;
    config.load( configFile );

    float scaling = config.get( "scale", 1.0f );
    Vec3 translation( config.get( "translate.x", 0.0f ),
                      config.get( "translate.y", 0.0f ),
                      config.get( "translate.z", 0.0f ) );
    Vec3 crouchTranslation( config.get( "crouchTranslate.x", 0.0f ),
                            config.get( "crouchTranslate.y", 0.0f ),
                            config.get( "crouchTranslate.z", 0.0f ) );
    config.clear();

    if( scaling != 1.0f ) {
      scale( scaling );
    }
    translate( translation );
    translate( ANIM_CROUCH_STAND,  crouchTranslation );
    translate( ANIM_CROUCH_WALK,   crouchTranslation );
    translate( ANIM_CROUCH_ATTACK, crouchTranslation );
    translate( ANIM_CROUCH_PAIN,   crouchTranslation );
    translate( ANIM_CROUCH_DEATH,  crouchTranslation );

    if( texId == 0 ) {
      return false;
    }
    return true;
  }

  void MD2::scale( float scale )
  {
    int max = nVerts * nFrames;

    for( int i = 0; i < max; i++ ) {
      verts[i] *= scale;
    }
  }

  void MD2::translate( const Vec3 &t )
  {
    int max = nVerts * nFrames;

    for( int i = 0; i < max; i++ ) {
      verts[i] += t;
    }
  }

  void MD2::translate( int animType, const Vec3 &t )
  {
    int start = animList[animType].firstFrame * nVerts;
    int max = ( animList[animType].lastFrame + 1 ) * nVerts;

    for( int i = start; i < max; i++ ) {
      verts[i] += t;
    }
  }

  void MD2::animate( AnimState *anim, float time )
  {
    anim->currTime += time;

    if( anim->currTime - anim->oldTime > anim->frameTime ) {
      anim->currFrame = anim->nextFrame;

      if( anim->nextFrame >= anim->endFrame ) {
        anim->nextFrame = anim->startFrame;
      }
      else {
        anim->nextFrame++;
      }
      anim->oldTime = anim->currTime;
    }

    animInterpol = anim->fps * ( anim->currTime - anim->oldTime );
  }

  void MD2::interpolate( AnimState *anim, Vec3 *vertList )
  {
    Vec3 *currFrame;
    Vec3 *nextFrame;

    currFrame = &verts[nVerts * anim->currFrame];
    nextFrame = &verts[nVerts * anim->nextFrame];

    for( int i = 0; i < nVerts; i++ ) {
      vertList[i] = currFrame[i] + animInterpol * ( nextFrame[i] - currFrame[i] );
    }
  }

  void MD2::drawFrame( int frame )
  {
    Vec3 *vertList = &verts[nVerts * frame];
    int *pCmd = glCmds;

    glFrontFace( GL_CW );
    glRotatef( 90.0f, 0.0f, 0.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, texId );

    while( int i = *( pCmd++ ) ) {
      if( i < 0 ) {
        glBegin( GL_TRIANGLE_FAN );
        i = -i;
      }
      else {
        glBegin( GL_TRIANGLE_STRIP );
      }
      for( ; i > 0; i--, pCmd += 3 ) {
        glNormal3fv( anorms[ lightNormals[ pCmd[2]] ] );
        glTexCoord2f( ( (float*) pCmd )[0], ( (float*) pCmd )[1] );
        glVertex3fv( vertList[pCmd[2]] );
      }
      glEnd();
    }
    glFrontFace( GL_CCW );
  }

  void MD2::draw( AnimState *anim )
  {
    static Vec3 vertList[MD2_MAX_VERTS];
    int *pCmd = glCmds;

    animate( anim, timer.frameTime );
    interpolate( anim, vertList );

    glFrontFace( GL_CW );
    glRotatef( 90.0f, 0.0f, 0.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, texId );

    while( int i = *( pCmd++ ) ) {
      if( i < 0 ) {
        glBegin( GL_TRIANGLE_FAN );
        i = -i;
      }
      else {
        glBegin( GL_TRIANGLE_STRIP );
      }
      for( ; i > 0; i--, pCmd += 3 ) {
        glNormal3fv( anorms[ lightNormals[ pCmd[2]] ] );
        glTexCoord2f( ( (float*) pCmd )[0], ( (float*) pCmd )[1] );
        glVertex3fv( vertList[pCmd[2]] );
      }
      glEnd();
    }
    glFrontFace( GL_CCW );
  }

  uint MD2::genList( const char *path )
  {
    MD2 md2;

    md2.load( path );

    uint list = context.genList();

    glNewList( list, GL_COMPILE );
      md2.drawFrame( 0 );
    glEndList();

    return list;
  }

  void MD2::free()
  {
    if( verts != null ) {
      delete[] verts;
      verts = null;
    }
    if( glCmds != null ) {
      delete[] glCmds;
      glCmds = null;
    }
    if( lightNormals != null ) {
      delete[] lightNormals;
      lightNormals = null;
    }
  }

}
}
