/*
 *  GalileoFrame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/modules/GalileoFrame.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"

#include "client/modules/GalileoModule.hpp"
#include "client/modules/QuestModule.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  uint GalileoFrame::loadTexture( const char* path ) const
  {
    Buffer buffer;

    if( !buffer.read( path ) ) {
      throw Exception( "Failed reading galileo texture '" + String( path ) + "'" );
    }

    InputStream istream = buffer.inputStream();
    return context.readTexture( &istream );
  }

  bool GalileoFrame::onMouseEvent()
  {
    return isVisible ? Frame::onMouseEvent() : false;
  }

  void GalileoFrame::onDraw()
  {
    if( camera.state == Camera::BOT && camera.botObj != null ) {
      const Vector<int>& items = camera.botObj->items;

      for( int i = 0; i < items.length(); ++i ) {
        const Object* item = orbis.objects[ items[i] ];

        if( item != null && item->clazz->name.equals( "galileo" ) ) {
          goto galileoEnabled;
        }
      }

      isVisible = false;
      return;
    }

    galileoEnabled:;

    isVisible = true;

    if( mapTexId == 0 ) {
      mapTexId = loadTexture( "ui/galileo/" + library.terras[orbis.terra.id].name + ".ozcTex" );
    }

    float pX;
    float pY;
    float h;

    if( camera.state == Camera::BOT && camera.botObj != null ) {
      pX = camera.botObj->p.x;
      pY = camera.botObj->p.y;
      h  = camera.botObj->h;
    }
    else {
      pX = camera.p.x;
      pY = camera.p.y;
      h  = camera.h;
    }

    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.6f );
    glBindTexture( GL_TEXTURE_2D, mapTexId );
    fill( 0, 0, width, height );

    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );

    float oX      = float( x );
    float oY      = float( y );
    float fWidth  = float( width );
    float fHeight = float( height );

    const QuestFrame* questFrame = questModule.questFrame;
    if( questFrame != null && questFrame->currentQuest != -1 ) {
      const Quest& quest = questModule.quests[questFrame->currentQuest];

      glBindTexture( GL_TEXTURE_2D, markerTexId );

      float mapX = oX + ( Orbis::DIM + quest.place.x ) / ( 2.0f*Orbis::DIM ) * fWidth;
      float mapY = oY + ( Orbis::DIM + quest.place.y ) / ( 2.0f*Orbis::DIM ) * fHeight;

      tf.model = Mat44::translation( Vec3( mapX, mapY, 0.0f ) );
      tf.model.scale( Vec3( 16.0f, 16.0f, 0.0f ) );
      tf.model.translate( Vec3( -0.5f, -0.5f, 0.0f ) );
      tf.apply();

      glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    }

    glBindTexture( GL_TEXTURE_2D, arrowTexId );

    float mapX = oX + ( Orbis::DIM + pX ) / ( 2.0f*Orbis::DIM ) * fWidth;
    float mapY = oY + ( Orbis::DIM + pY ) / ( 2.0f*Orbis::DIM ) * fHeight;

    tf.model = Mat44::translation( Vec3( mapX, mapY, 0.0f ) );
    tf.model.rotateZ( h );
    tf.model.scale( Vec3( 10.0f, 10.0f, 0.0f ) );
    tf.model.translate( Vec3( -0.5f, -0.5f, 0.0f ) );
    tf.apply();

    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    glBindTexture( GL_TEXTURE_2D, 0 );
  }

  GalileoFrame::GalileoFrame() :
      Frame( 8, -60 - Font::INFOS[Font::SMALL].height - Font::INFOS[Font::LARGE].height,
             240, 232 - Font::INFOS[Font::LARGE].height, "" ),
      mapTexId( 0 ), arrowTexId( 0 ), markerTexId( 0 ), isVisible( true )

  {
    arrowTexId = loadTexture( "ui/galileo/arrow.ozcTex" );
    markerTexId = loadTexture( "ui/galileo/marker.ozcTex" );
  }

  GalileoFrame::~GalileoFrame()
  {
    if( mapTexId != 0 ) {
      glDeleteTextures( 1, &mapTexId );
    }
    if( arrowTexId != 0 ) {
      glDeleteTextures( 1, &arrowTexId );
    }
    if( markerTexId != 0 ) {
      glDeleteTextures( 1, &markerTexId );
    }
  }

}
}
}
