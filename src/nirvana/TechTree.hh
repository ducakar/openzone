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
 * @file nirvana/TechTree.hh
 */

#pragma once

#include <nirvana/common.hh>

namespace oz
{

class TechTree
{
  public:

    struct Node
    {
      static const int MAX_DEPS = 8;

      enum Type
      {
        TECHNOLOGY,
        BUILDING,
        UNIT,
        ITEM
      };

      Type                   type;
      int                    price;
      float                  time;
      String                 name;
      String                 title;
      String                 description;
      union
      {
        const BSP*           building;
        const ObjectClass*   unit;
        const ObjectClass*   item;
      };
      SList<Node*, MAX_DEPS> requires;
      float                  progress;
    };

  private:

    List<Node> nodes;

    Node* findNode( const char* name );

  public:

    List<const BSP*>         allowedBuildings;
    List<const ObjectClass*> allowedUnits;
    List<const ObjectClass*> allowedItems;

    void update();

    void read( InputStream* istream );
    void write( OutputStream* ostream ) const;

    void load();
    void unload();

};

extern TechTree techTree;

}
