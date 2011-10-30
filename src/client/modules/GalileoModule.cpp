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
#include "client/OpenGL.hpp"

#include "client/ui/UI.hpp"

#include "luamacros.hpp"

namespace oz
{
namespace client
{

GalileoModule galileoModule;

GalileoModule::GalileoModule() : galileoFrame( null )
{}

void GalileoModule::load()
{
  galileoFrame = new ui::GalileoFrame();
  ui::ui.root->add( galileoFrame );
  ui::ui.root->focus( ui::ui.loadingScreen );
}

void GalileoModule::unload()
{
  if( galileoFrame != null ) {
    ui::ui.root->remove( galileoFrame );
    galileoFrame = null;
  }
}

}
}
