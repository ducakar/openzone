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
 * @file client/ui/Bar.cc
 */

#include <client/ui/Bar.hh>

#include <client/Shape.hh>

namespace oz
{
namespace client
{
namespace ui
{

Bar::Bar( const Style::Bar* style_ ) :
  style( style_ )
{}

void Bar::setStyle( const Style::Bar* style_ )
{
  style = style_;
}

void Bar::draw( const Area* area, float ratio ) const
{
  hard_assert( style != nullptr );

  draw( area, style->x, style->y, style->w, style->h, ratio );
}

void Bar::draw( const Area* area, int barX, int barY, int barWidth, int barHeight,
                float ratio ) const
{
  hard_assert( style != nullptr );

  int x = barX == Area::CENTRE ? ( area->width - barWidth ) / 2 :
          barX < 0 ? area->width - barWidth + barX : barX;
  int y = barY == Area::CENTRE ? ( area->height - barHeight ) / 2 :
          barY < 0 ? area->height - barHeight + barY : barY;

  int width = Math::lround( float( barWidth - 2 ) * ratio );

  shape.colour( style->border );
  shape.rect( x, y, barWidth, barHeight );

  shape.colour( Math::mix( style->minColour, style->maxColour, ratio ) );
  shape.fill( x + 1, y + 1, width, barHeight - 2 );

  shape.colour( style->background );
  shape.fill( x + 1 + width, y + 1, barWidth - 2 - width, barHeight - 2 );
}


}
}
}
