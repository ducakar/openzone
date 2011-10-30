/*
 *  GalileoModule.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/Module.hpp"

#include "client/modules/GalileoFrame.hpp"

namespace oz
{
namespace client
{

class GalileoModule : public Module
{
  private:

    ui::GalileoFrame* galileoFrame;

  public:

    GalileoModule();

    virtual void load();
    virtual void unload();

};

extern GalileoModule galileoModule;

}
}
