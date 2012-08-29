/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/ui/Button.hh
 */

#pragma once

#include <client/ui/Area.hh>
#include <client/ui/Label.hh>

namespace oz
{
namespace client
{
namespace ui
{

class Button : public Area
{
  public:

    typedef void Callback( Button* sender );

  private:

    Label     label;
    Callback* callback;
    bool      isHighlighted;
    bool      isClicked;

  protected:

    void onVisibilityChange( bool doShow ) override;
    bool onMouseEvent() override;
    void onDraw() override;

  public:

    explicit Button( const char* text, Callback* callback, int width, int height );

    void setLabel( const char* text );
    void setCallback( Callback* callback );

};

}
}
}
