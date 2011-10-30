/*
 *  BuildMenu.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/ui/Frame.hpp"
#include "client/ui/Button.hpp"

namespace oz
{
namespace client
{
namespace ui
{

class BuildButton : public Button
{
  public:

    String className;

    explicit BuildButton( const char* className, Callback* callback, int width, int height );

};

class BuildMenu : public Frame
{
  private:

    static void createObject( Button* button );

  public:

    BuildMenu();

};

}
}
}
