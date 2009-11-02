/*
 *  MD2.cpp
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "MD2.h"

#include "Context.h"

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
    // first, last, fps, repeat
    {   0,  39,  9.0f, 1 },   // STAND
    {  40,  45, 10.0f, 1 },   // RUN
    {  46,  53, 10.0f, 1 },   // ATTACK
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
    { 160, 168, 10.0f, 1 },   // CROUCH_ATTACK
    { 196, 172,  7.0f, 1 },   // CROUCH_PAIN
    { 173, 177,  5.0f, 1 },   // CROUCH_DEATH
    { 178, 183,  7.0f, 0 },   // DEATH_FALLBACK
    { 184, 189,  7.0f, 0 },   // DEATH_FALLFORWARD
    { 190, 197,  7.0f, 0 },   // DEATH_FALLBACKSLOW
    {   0, 197,  7.0f, 1 }    // FULL
  };

  Vec3 MD2::vertList[MAX_VERTS];
  
  void MD2::init()
  {
    for( uint i = 0; i < sizeof( anorms ) / sizeof( anorms[0] ); i++ ) {
      float x = -anorms[i][1];
      float y =  anorms[i][0];

      anorms[i][0] = x;
      anorms[i][1] = y;
    }
  }

  MD2::MD2( const char *name_ )
  {
    FILE      *file;
    MD2Header header;
    char      *buffer;
    MD2Frame  *pFrame;
    Vec3      *pVerts;
    int       *pNormals;

    name = name_;

    String sPath = "mdl" OZ_DIRDEL + name;
    String modelFile = sPath + OZ_DIRDEL "tris.md2";
#ifdef OZ_MINGW32
    String skinFile = sPath + OZ_DIRDEL "skin.png";
#else
    String skinFile = sPath + OZ_DIRDEL "skin.jpg";
#endif
    String configFile = sPath + OZ_DIRDEL "config.rc";

    log.print( "Loading MD2 model '%s' ...", name.cstr() );

    file = fopen( modelFile.cstr(), "rb" );
    if( file == null ) {
      log.printEnd( " No such file" );
      throw Exception( "MD2 model loading error" );
    }

    fread( &header, 1, sizeof( header ), file );
    if( header.id != FOURCC( 'I', 'D', 'P', '2' ) || header.version != 8 ) {
      fclose( file );
      log.printEnd( " Invalid format" );
      throw Exception( "MD2 model loading error" );
    }

    nFrames = header.nFrames;
    nVerts = header.nVerts;

    if( nFrames <= 0 || nVerts <= 0 ) {
      fclose( file );
      log.printEnd( " Format error" );
      throw Exception( "MD2 model loading error" );
    }

    verts = new Vec3[nVerts * nFrames];
    glCmds = new int[header.nGlCmds];
    lightNormals = new int[nVerts * nFrames];

    buffer = new char[nFrames * header.framesize];

    fseek( file, header.offFrames, SEEK_SET );
    fread( buffer, 1, nFrames * header.framesize, file );

    fseek( file, header.offGLCmds, SEEK_SET );
    fread( glCmds, 1, header.nGlCmds * sizeof( int ), file );

    for( int i = 0; i < nFrames; i++ ) {
      pFrame = reinterpret_cast<MD2Frame*>( &buffer[header.framesize * i] );
      pVerts = &verts[nVerts * i];
      pNormals = &lightNormals[nVerts * i];

      for( int j = 0; j < nVerts; j++ ) {
        pVerts[j] = Vec3(
          ( static_cast<float>( pFrame->verts[j].v[1] ) * -pFrame->scale.y ) - pFrame->translate.y,
          ( static_cast<float>( pFrame->verts[j].v[0] ) *  pFrame->scale.x ) + pFrame->translate.x,
          ( static_cast<float>( pFrame->verts[j].v[2] ) *  pFrame->scale.z ) + pFrame->translate.z );

        pNormals[j] = pFrame->verts[j].iLightNormal;
      }
    }
    delete[] buffer;
    fclose( file );

    log.printEnd( "OK" );

    texId = context.loadTexture( skinFile, true );

    Config config;
    config.load( configFile );

    float scaling = config.get( "scale", 0.03f );
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

    if( !crouchTranslation.isZero() ) {
      translate( ANIM_CROUCH_STAND,  crouchTranslation );
      translate( ANIM_CROUCH_WALK,   crouchTranslation );
      translate( ANIM_CROUCH_ATTACK, crouchTranslation );
      translate( ANIM_CROUCH_PAIN,   crouchTranslation );
      translate( ANIM_CROUCH_DEATH,  crouchTranslation );
    }

    if( texId == 0 ) {
      throw Exception( "MD2 model loading error" );
    }
  }

  MD2::~MD2()
  {
    log.print( "Unloading MD2 model '%s' ...", name.cstr() );

    context.freeTexture( texId );
    delete[] verts;
    delete[] glCmds;
    delete[] lightNormals;

    log.printEnd( " OK" );

    assert( glGetError() == GL_NO_ERROR );
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

  void MD2::interpolate( AnimState *anim, float dt ) const
  {
    anim->currTime += dt;

    while( anim->currTime > anim->frameTime ) {
      anim->currTime -= anim->frameTime;
      anim->currFrame = anim->nextFrame;

      if( anim->nextFrame < anim->endFrame ) {
        anim->nextFrame++;
      }
      else if( anim->repeat ) {
        anim->nextFrame = anim->startFrame;
      }
    }

    const Vec3 *currFrame = &verts[nVerts * anim->currFrame];
    const Vec3 *nextFrame = &verts[nVerts * anim->nextFrame];

    float t1 = anim->fps * anim->currTime;
    float t2 = 1.0f - t1;

    for( int i = 0; i < nVerts; i++ ) {
      vertList[i] = t2 * currFrame[i] + t1 * nextFrame[i];
    }
  }

  void MD2::drawFrame( int frame ) const
  {
    const Vec3 *vertList = &verts[nVerts * frame];
    const int  *pCmd     = glCmds;

    glFrontFace( GL_CW );
    glBindTexture( GL_TEXTURE_2D, texId );

    glPushMatrix();

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
        glTexCoord2f( reinterpret_cast<const float*>( pCmd )[0],
                      reinterpret_cast<const float*>( pCmd )[1] );
        glVertex3fv( vertList[pCmd[2]] );
      }
      glEnd();
    }

    glPopMatrix();
    glFrontFace( GL_CCW );
  }

  void MD2::draw( AnimState *anim ) const
  {
    const int *pCmd = glCmds;

    interpolate( anim, timer.frameTime );

    glFrontFace( GL_CW );
    glBindTexture( GL_TEXTURE_2D, texId );

    glPushMatrix();

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
        glTexCoord2f( reinterpret_cast<const float*>( pCmd )[0],
                      reinterpret_cast<const float*>( pCmd )[1] );
        glVertex3fv( vertList[pCmd[2]] );
      }
      glEnd();
    }

    glPopMatrix();
    glFrontFace( GL_CCW );
  }

  void MD2::genList()
  {
    list = glGenLists( 1 );
    glNewList( list, GL_COMPILE );
      drawFrame( 0 );
    glEndList();
  }

}
}
