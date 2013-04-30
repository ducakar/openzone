/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/ui/GalileoFrame.hh
 */

#pragma once

#include <client/ui/Frame.hh>

namespace oz
{
namespace client
{
namespace ui
{

class QuestFrame;

class GalileoFrame : public Frame
{
  private:

    GLTexture mapTex;
    GLTexture arrowTex;
    GLTexture markerTex;

    int       normalX;
    int       normalY;
    int       normalWidth;
    int       normalHeight;

    int       maximisedX;
    int       maximisedY;
    int       maximisedWidth;
    int       maximisedHeight;

    Vec4      colour;

  public:

    bool isMaximised;

  protected:

    void onUpdate() override;
    void onReposition() override;
    void onDraw() override;

  public:

    explicit GalileoFrame();

    void setMaximised( bool doMaximise );

};

}
}
}
