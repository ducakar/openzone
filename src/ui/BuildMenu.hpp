/*
 *  BuildMenu.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "ui/Frame.hpp"
#include "ui/Button.hpp"

namespace oz
{
namespace client
{
namespace ui
{

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
