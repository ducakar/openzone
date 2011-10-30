/*
 *  MenuStage.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/Stage.hpp"

#include "client/ui/MainMenu.hpp"

namespace oz
{
namespace client
{

class MenuStage : public Stage
{
  public:

    bool doExit;

    virtual bool update();
    virtual void present();

    virtual void load();
    virtual void unload();

    virtual void init();
    virtual void free();

};

extern MenuStage menuStage;

}
}
