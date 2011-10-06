/*
 *  GalileoModule.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/modules/GalileoModule.hpp"

#include "client/Context.hpp"

#include "client/ui/UI.hpp"

#include "client/OpenGL.hpp"

#include "luamacros.hpp"

namespace oz
{
namespace client
{

  GalileoModule galileoModule;

#ifndef OZ_TOOLS

  GalileoModule::GalileoModule() : galileoFrame( null )
  {}

  void GalileoModule::load()
  {
    galileoFrame = new ui::GalileoFrame();
    ui::ui.root->add( galileoFrame );
  }

  void GalileoModule::unload()
  {
    if( galileoFrame != null ) {
      ui::ui.root->remove( galileoFrame );
      galileoFrame = null;
    }
  }

#else

  void GalileoModule::prebuild()
  {
    Config terraConfig;
    Buffer buffer( 1024 * 1024 );

    for( int i = 0; i < library.terras.length(); ++i ) {
      const String& name = library.terras[i].name;

      terraConfig.load( "terra/" + name + ".rc" );

      String srcTextureFile = String( "terra/" ) + terraConfig.get( "mapTexture", "" );
      String destTextureFile = "ui/galileo/" + name + ".ozcTex";

      OutputStream ostream = buffer.outputStream();

      uint id = context.loadRawTexture( srcTextureFile, true, GL_LINEAR, GL_LINEAR );
      context.writeTexture( id, &ostream );

      terraConfig.clear();

      buffer.write( destTextureFile, ostream.length() );
    }
  }

#endif

}
}
