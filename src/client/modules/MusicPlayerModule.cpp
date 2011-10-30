/*
 *  MusicPlayerModule.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/modules/MusicPlayerModule.hpp"

#include "client/modules/MusicPlayer.hpp"

#include "client/ui/UI.hpp"

namespace oz
{
namespace client
{

MusicPlayerModule musicPlayerModule;

MusicPlayerModule::MusicPlayerModule() : musicPlayer( null )
{}

void MusicPlayerModule::load()
{
  musicPlayer = new ui::MusicPlayer();
  ui::ui.root->add( musicPlayer );
  ui::ui.root->focus( ui::ui.loadingScreen );
}

void MusicPlayerModule::unload()
{
  if( musicPlayer != null ) {
    ui::ui.root->remove( musicPlayer );
    musicPlayer = null;
  }
}

}
}
