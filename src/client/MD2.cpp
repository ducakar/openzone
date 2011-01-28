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
    ubyte v[3];
    ubyte lightNormal;
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

  Vec3 MD2::anorms[] =
  {
    // normal vectors
    Vec3( -0.525731f,  0.000000f,  0.850651f ),
    Vec3( -0.442863f,  0.238856f,  0.864188f ),
    Vec3( -0.295242f,  0.000000f,  0.955423f ),
    Vec3( -0.309017f,  0.500000f,  0.809017f ),
    Vec3( -0.162460f,  0.262866f,  0.951056f ),
    Vec3(  0.000000f,  0.000000f,  1.000000f ),
    Vec3(  0.000000f,  0.850651f,  0.525731f ),
    Vec3( -0.147621f,  0.716567f,  0.681718f ),
    Vec3(  0.147621f,  0.716567f,  0.681718f ),
    Vec3(  0.000000f,  0.525731f,  0.850651f ),
    Vec3(  0.309017f,  0.500000f,  0.809017f ),
    Vec3(  0.525731f,  0.000000f,  0.850651f ),
    Vec3(  0.295242f,  0.000000f,  0.955423f ),
    Vec3(  0.442863f,  0.238856f,  0.864188f ),
    Vec3(  0.162460f,  0.262866f,  0.951056f ),
    Vec3( -0.681718f,  0.147621f,  0.716567f ),
    Vec3( -0.809017f,  0.309017f,  0.500000f ),
    Vec3( -0.587785f,  0.425325f,  0.688191f ),
    Vec3( -0.850651f,  0.525731f,  0.000000f ),
    Vec3( -0.864188f,  0.442863f,  0.238856f ),
    Vec3( -0.716567f,  0.681718f,  0.147621f ),
    Vec3( -0.688191f,  0.587785f,  0.425325f ),
    Vec3( -0.500000f,  0.809017f,  0.309017f ),
    Vec3( -0.238856f,  0.864188f,  0.442863f ),
    Vec3( -0.425325f,  0.688191f,  0.587785f ),
    Vec3( -0.716567f,  0.681718f, -0.147621f ),
    Vec3( -0.500000f,  0.809017f, -0.309017f ),
    Vec3( -0.525731f,  0.850651f,  0.000000f ),
    Vec3(  0.000000f,  0.850651f, -0.525731f ),
    Vec3( -0.238856f,  0.864188f, -0.442863f ),
    Vec3(  0.000000f,  0.955423f, -0.295242f ),
    Vec3( -0.262866f,  0.951056f, -0.162460f ),
    Vec3(  0.000000f,  1.000000f,  0.000000f ),
    Vec3(  0.000000f,  0.955423f,  0.295242f ),
    Vec3( -0.262866f,  0.951056f,  0.162460f ),
    Vec3(  0.238856f,  0.864188f,  0.442863f ),
    Vec3(  0.262866f,  0.951056f,  0.162460f ),
    Vec3(  0.500000f,  0.809017f,  0.309017f ),
    Vec3(  0.238856f,  0.864188f, -0.442863f ),
    Vec3(  0.262866f,  0.951056f, -0.162460f ),
    Vec3(  0.500000f,  0.809017f, -0.309017f ),
    Vec3(  0.850651f,  0.525731f,  0.000000f ),
    Vec3(  0.716567f,  0.681718f,  0.147621f ),
    Vec3(  0.716567f,  0.681718f, -0.147621f ),
    Vec3(  0.525731f,  0.850651f,  0.000000f ),
    Vec3(  0.425325f,  0.688191f,  0.587785f ),
    Vec3(  0.864188f,  0.442863f,  0.238856f ),
    Vec3(  0.688191f,  0.587785f,  0.425325f ),
    Vec3(  0.809017f,  0.309017f,  0.500000f ),
    Vec3(  0.681718f,  0.147621f,  0.716567f ),
    Vec3(  0.587785f,  0.425325f,  0.688191f ),
    Vec3(  0.955423f,  0.295242f,  0.000000f ),
    Vec3(  1.000000f,  0.000000f,  0.000000f ),
    Vec3(  0.951056f,  0.162460f,  0.262866f ),
    Vec3(  0.850651f, -0.525731f,  0.000000f ),
    Vec3(  0.955423f, -0.295242f,  0.000000f ),
    Vec3(  0.864188f, -0.442863f,  0.238856f ),
    Vec3(  0.951056f, -0.162460f,  0.262866f ),
    Vec3(  0.809017f, -0.309017f,  0.500000f ),
    Vec3(  0.681718f, -0.147621f,  0.716567f ),
    Vec3(  0.850651f,  0.000000f,  0.525731f ),
    Vec3(  0.864188f,  0.442863f, -0.238856f ),
    Vec3(  0.809017f,  0.309017f, -0.500000f ),
    Vec3(  0.951056f,  0.162460f, -0.262866f ),
    Vec3(  0.525731f,  0.000000f, -0.850651f ),
    Vec3(  0.681718f,  0.147621f, -0.716567f ),
    Vec3(  0.681718f, -0.147621f, -0.716567f ),
    Vec3(  0.850651f,  0.000000f, -0.525731f ),
    Vec3(  0.809017f, -0.309017f, -0.500000f ),
    Vec3(  0.864188f, -0.442863f, -0.238856f ),
    Vec3(  0.951056f, -0.162460f, -0.262866f ),
    Vec3(  0.147621f,  0.716567f, -0.681718f ),
    Vec3(  0.309017f,  0.500000f, -0.809017f ),
    Vec3(  0.425325f,  0.688191f, -0.587785f ),
    Vec3(  0.442863f,  0.238856f, -0.864188f ),
    Vec3(  0.587785f,  0.425325f, -0.688191f ),
    Vec3(  0.688191f,  0.587785f, -0.425325f ),
    Vec3( -0.147621f,  0.716567f, -0.681718f ),
    Vec3( -0.309017f,  0.500000f, -0.809017f ),
    Vec3(  0.000000f,  0.525731f, -0.850651f ),
    Vec3( -0.525731f,  0.000000f, -0.850651f ),
    Vec3( -0.442863f,  0.238856f, -0.864188f ),
    Vec3( -0.295242f,  0.000000f, -0.955423f ),
    Vec3( -0.162460f,  0.262866f, -0.951056f ),
    Vec3(  0.000000f,  0.000000f, -1.000000f ),
    Vec3(  0.295242f,  0.000000f, -0.955423f ),
    Vec3(  0.162460f,  0.262866f, -0.951056f ),
    Vec3( -0.442863f, -0.238856f, -0.864188f ),
    Vec3( -0.309017f, -0.500000f, -0.809017f ),
    Vec3( -0.162460f, -0.262866f, -0.951056f ),
    Vec3(  0.000000f, -0.850651f, -0.525731f ),
    Vec3( -0.147621f, -0.716567f, -0.681718f ),
    Vec3(  0.147621f, -0.716567f, -0.681718f ),
    Vec3(  0.000000f, -0.525731f, -0.850651f ),
    Vec3(  0.309017f, -0.500000f, -0.809017f ),
    Vec3(  0.442863f, -0.238856f, -0.864188f ),
    Vec3(  0.162460f, -0.262866f, -0.951056f ),
    Vec3(  0.238856f, -0.864188f, -0.442863f ),
    Vec3(  0.500000f, -0.809017f, -0.309017f ),
    Vec3(  0.425325f, -0.688191f, -0.587785f ),
    Vec3(  0.716567f, -0.681718f, -0.147621f ),
    Vec3(  0.688191f, -0.587785f, -0.425325f ),
    Vec3(  0.587785f, -0.425325f, -0.688191f ),
    Vec3(  0.000000f, -0.955423f, -0.295242f ),
    Vec3(  0.000000f, -1.000000f,  0.000000f ),
    Vec3(  0.262866f, -0.951056f, -0.162460f ),
    Vec3(  0.000000f, -0.850651f,  0.525731f ),
    Vec3(  0.000000f, -0.955423f,  0.295242f ),
    Vec3(  0.238856f, -0.864188f,  0.442863f ),
    Vec3(  0.262866f, -0.951056f,  0.162460f ),
    Vec3(  0.500000f, -0.809017f,  0.309017f ),
    Vec3(  0.716567f, -0.681718f,  0.147621f ),
    Vec3(  0.525731f, -0.850651f,  0.000000f ),
    Vec3( -0.238856f, -0.864188f, -0.442863f ),
    Vec3( -0.500000f, -0.809017f, -0.309017f ),
    Vec3( -0.262866f, -0.951056f, -0.162460f ),
    Vec3( -0.850651f, -0.525731f,  0.000000f ),
    Vec3( -0.716567f, -0.681718f, -0.147621f ),
    Vec3( -0.716567f, -0.681718f,  0.147621f ),
    Vec3( -0.525731f, -0.850651f,  0.000000f ),
    Vec3( -0.500000f, -0.809017f,  0.309017f ),
    Vec3( -0.238856f, -0.864188f,  0.442863f ),
    Vec3( -0.262866f, -0.951056f,  0.162460f ),
    Vec3( -0.864188f, -0.442863f,  0.238856f ),
    Vec3( -0.809017f, -0.309017f,  0.500000f ),
    Vec3( -0.688191f, -0.587785f,  0.425325f ),
    Vec3( -0.681718f, -0.147621f,  0.716567f ),
    Vec3( -0.442863f, -0.238856f,  0.864188f ),
    Vec3( -0.587785f, -0.425325f,  0.688191f ),
    Vec3( -0.309017f, -0.500000f,  0.809017f ),
    Vec3( -0.147621f, -0.716567f,  0.681718f ),
    Vec3( -0.425325f, -0.688191f,  0.587785f ),
    Vec3( -0.162460f, -0.262866f,  0.951056f ),
    Vec3(  0.442863f, -0.238856f,  0.864188f ),
    Vec3(  0.162460f, -0.262866f,  0.951056f ),
    Vec3(  0.309017f, -0.500000f,  0.809017f ),
    Vec3(  0.147621f, -0.716567f,  0.681718f ),
    Vec3(  0.000000f, -0.525731f,  0.850651f ),
    Vec3(  0.425325f, -0.688191f,  0.587785f ),
    Vec3(  0.587785f, -0.425325f,  0.688191f ),
    Vec3(  0.688191f, -0.587785f,  0.425325f ),
    Vec3( -0.955423f,  0.295242f,  0.000000f ),
    Vec3( -0.951056f,  0.162460f,  0.262866f ),
    Vec3( -1.000000f,  0.000000f,  0.000000f ),
    Vec3( -0.850651f,  0.000000f,  0.525731f ),
    Vec3( -0.955423f, -0.295242f,  0.000000f ),
    Vec3( -0.951056f, -0.162460f,  0.262866f ),
    Vec3( -0.864188f,  0.442863f, -0.238856f ),
    Vec3( -0.951056f,  0.162460f, -0.262866f ),
    Vec3( -0.809017f,  0.309017f, -0.500000f ),
    Vec3( -0.864188f, -0.442863f, -0.238856f ),
    Vec3( -0.951056f, -0.162460f, -0.262866f ),
    Vec3( -0.809017f, -0.309017f, -0.500000f ),
    Vec3( -0.681718f,  0.147621f, -0.716567f ),
    Vec3( -0.681718f, -0.147621f, -0.716567f ),
    Vec3( -0.850651f,  0.000000f, -0.525731f ),
    Vec3( -0.688191f,  0.587785f, -0.425325f ),
    Vec3( -0.587785f,  0.425325f, -0.688191f ),
    Vec3( -0.425325f,  0.688191f, -0.587785f ),
    Vec3( -0.425325f, -0.688191f, -0.587785f ),
    Vec3( -0.587785f, -0.425325f, -0.688191f ),
    Vec3( -0.688191f, -0.587785f, -0.425325f )
  };

  MD2::Anim MD2::animList[] =
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

  Vec3 MD2::vertList[MAX_VERTS];

  void MD2::interpolate( const AnimState* anim ) const
  {
    const Vec3* currFrame = &verts[nVerts * anim->currFrame];
    const Vec3* nextFrame = &verts[nVerts * anim->nextFrame];

    float t1 = anim->fps * anim->currTime;
    float t2 = 1.0f - t1;

    for( int i = 0; i < nVerts; ++i ) {
      vertList[i] = t2 * currFrame[i] + t1 * nextFrame[i];
    }
  }

  void MD2::init()
  {
    for( uint i = 0; i < sizeof anorms / sizeof anorms[0]; ++i ) {
      float x = -anorms[i][1];
      float y =  anorms[i][0];

      anorms[i][0] = x;
      anorms[i][1] = y;
    }
  }

  MD2::MD2( const char* name_ )
  {
    FILE*     file;
    MD2Header header;
    char*     buffer;
    MD2Frame* pFrame;
    Vec3*     pVerts;
    int*      pNormals;

    name = name_;

    String sPath = "mdl/" + name;
    String modelFile = sPath + "/tris.md2";
    String skinFile = sPath + "/skin.jpg";
    String configFile = sPath + "/config.rc";

    log.print( "Loading MD2 model '%s' ...", name.cstr() );

    file = fopen( modelFile.cstr(), "rb" );
    if( file == null ) {
      log.printEnd( " No such file" );
      throw Exception( "MD2 model loading error" );
    }

    fread( &header, 1, sizeof( MD2Header ), file );
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

    buffer = new char[nFrames * header.frameSize];

    fseek( file, header.offFrames, SEEK_SET );
    fread( buffer, 1, nFrames * header.frameSize, file );

    fseek( file, header.offGLCmds, SEEK_SET );
    fread( glCmds, 1, header.nGlCmds * sizeof( int ), file );

    for( int i = 0; i < nFrames; ++i ) {
      pFrame = reinterpret_cast<MD2Frame*>( &buffer[header.frameSize * i] );
      pVerts = &verts[nVerts * i];
      pNormals = &lightNormals[nVerts * i];

      for( int j = 0; j < nVerts; ++j ) {
        pVerts[j] = Vec3(
          ( float( pFrame->verts[j].v[1] ) * -pFrame->scale[1] ) - pFrame->translate[1],
          ( float( pFrame->verts[j].v[0] ) *  pFrame->scale[0] ) + pFrame->translate[0],
          ( float( pFrame->verts[j].v[2] ) *  pFrame->scale[2] ) + pFrame->translate[2] );

        pNormals[j] = pFrame->verts[j].lightNormal;
      }
    }
    delete[] buffer;
    fclose( file );

    log.printEnd( "OK" );

    texId = context.loadTexture( skinFile, true );

    Config config;
    config.load( configFile );

    float scaling = config.get( "scale", 0.042f );
    Vec3 translation   = Vec3( config.get( "translate.x", 0.00f ),
                               config.get( "translate.y", 0.00f ),
                               config.get( "translate.z", 0.00f ) );
    Vec3 jumpTranslate = Vec3( config.get( "jumpTranslate.x", 0.00f ),
                               config.get( "jumpTranslate.y", 0.00f ),
                               config.get( "jumpTranslate.z", 0.00f ) );
    weaponTransl       = Vec3( config.get( "weaponTranslate.x", 0.00f ),
                               config.get( "weaponTranslate.y", 0.00f ),
                               config.get( "weaponTranslate.z", 0.00f ) );

    if( scaling != 1.0f ) {
      scale( scaling );
    }

    translate( translation );

    if( jumpTranslate != Vec3::ZERO && animList[ANIM_JUMP].lastFrame < nFrames ) {
      translate( ANIM_JUMP, jumpTranslate );
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

  void MD2::optimise()
  {
    Vector< Vector<int> > strips;
  }

  void MD2::scale( float scale )
  {
    int max = nVerts * nFrames;

    for( int i = 0; i < max; ++i ) {
      verts[i] *= scale;
    }
  }

  void MD2::translate( const Vec3& t )
  {
    int end = nVerts * nFrames;

    for( int i = 0; i < end; ++i ) {
      verts[i] += t;
    }
  }

  void MD2::translate( int animType, const Vec3& t )
  {
    int start = animList[animType].firstFrame * nVerts;
    int end = ( animList[animType].lastFrame + 1 ) * nVerts - 1;

    assert( end < nVerts * nFrames );

    for( int i = start; i <= end; ++i ) {
      verts[i] += t;
    }
  }

  void MD2::drawFrame( int frame ) const
  {
    const Vec3* vertList = &verts[nVerts * frame];
    const int*  pCmd     = glCmds;

    glFrontFace( GL_CW );
    glBindTexture( GL_TEXTURE_2D, texId );

    while( int i = *( pCmd ) ) {
      if( i < 0 ) {
        glBegin( GL_TRIANGLE_FAN );
        i = -i;
      }
      else {
        glBegin( GL_TRIANGLE_STRIP );
      }
      ++pCmd;
      for( ; i > 0; --i, pCmd += 3 ) {
        glNormal3fv( anorms[ lightNormals[ pCmd[2] ] ] );
        glTexCoord2f( reinterpret_cast<const float*>( pCmd )[0],
                      reinterpret_cast<const float*>( pCmd )[1] );
        glVertex3fv( vertList[pCmd[2]] );
      }
      glEnd();
    }

    glFrontFace( GL_CCW );
  }

  void MD2::draw( const AnimState* anim ) const
  {
    const int* pCmd = glCmds;

    interpolate( anim );

    glFrontFace( GL_CW );
    glBindTexture( GL_TEXTURE_2D, texId );

    while( int i = *( pCmd ) ) {
      if( i < 0 ) {
        glBegin( GL_TRIANGLE_FAN );
        i = -i;
      }
      else {
        glBegin( GL_TRIANGLE_STRIP );
      }
      ++pCmd;
      for( ; i > 0; --i, pCmd += 3 ) {
        glNormal3fv( anorms[ lightNormals[ pCmd[2] ] ] );
        glTexCoord2f( reinterpret_cast<const float*>( pCmd )[0],
                      reinterpret_cast<const float*>( pCmd )[1] );
        glVertex3fv( vertList[pCmd[2]] );
      }
      glEnd();
    }

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
