/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/modules/QuestFrame.hh
 */

#pragma once

#include "client/ui/Frame.hh"
#include "client/ui/Button.hh"
#include "client/ui/Text.hh"

namespace oz
{
namespace client
{
namespace ui
{

class GalileoFrame;

class QuestFrame : public Frame
{
  friend class GalileoFrame;

  private:

    static const String statusMessages[];

    Text description;
    int  lastState;
    int  currentQuest;
    int  contentHeight;
    bool isOpened;

    void updateTask();

    static void open( Button* sender );
    static void next( Button* sender );
    static void prev( Button* sender );

  protected:

    virtual void onDraw();

  public:

    QuestFrame();

};

}
}
}
