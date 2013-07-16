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
 * @file client/ui/ModelField.hh
 */

#pragma once

#include <client/ui/Area.hh>

namespace oz
{
namespace client
{
namespace ui
{

class ModelField : public Area
{
  public:

    typedef void Callback( ModelField* sender );

  private:

    static const float DEFAULT_ROTATION;
    static const float ROTATION_VEL;
    static const float ROTATION_SMOOTHING;

    Callback*      callback;
    const oz::BSP* bsp;

    float          defaultRot;
    float          currRot;
    float          nextRot;

    bool           isHighlighted;
    bool           isClicked;

  protected:

    void onVisibilityChange( bool doShow ) override;
    bool onMouseEvent() override;
    void onDraw() override;

  public:

    explicit ModelField( Callback* callback, int width, int height );

    void setCallback( Callback* callback );
    void setDefaultRotation( float defaultRotation );

    void setBSP( const oz::BSP* bsp );
    void setModel( int );

};

}
}
}
