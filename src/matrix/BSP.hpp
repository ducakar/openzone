/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/BSP.hpp
 */

#pragma once

#include "matrix/common.hpp"

namespace oz
{
namespace matrix
{

/**
 * Information about a BSP that must be available also when the BSP is not loaded.
 */
struct BSPClass
{
  Bounds      bounds; ///< Bounds.
  Vector<int> sounds; ///< Sound samples.
};

class BSP : public Bounds
{
  public:

    static const int MAX_BRUSHES = 256;

    /**
     * BSP node.
     */
    struct Node
    {
      int plane; ///< Separating plane index in <tt>planes</tt> array.

      int front; ///< Index of node on the positive side of the separating plane.
      int back;  ///< Index of node on the negative side of the separating plane.
    };

    /**
     * BSP leaf node.
     */
    struct Leaf
    {
      int firstBrush; ///< Index of the first brush index in <tt>leafBrushes</tt> array.
      int nBrushes;   ///< Number of brush indices.
    };

    /**
     * BSP brush (convex polytope).
     */
    struct Brush
    {
      int firstSide; ///< Index of the first plane index in <tt>bushSides</tt> array.
      int nSides;    ///< Number of plane indices.

      int material;  ///< Material bits (look <code>matrix::Material</code>.
    };

    /**
     * BSP model (doors, lifts etc.).
     */
    struct Model : Bounds
    {
      enum Type
      {
        IGNORING,
        CRUSHING,
        AUTO_DOOR
      };

      Vec3   move;        ///< Move vector (destination - original position), in BSP coordinate
                          ///< system.

      BSP*   bsp;         ///< Pointer to the parent BSP.

      int    firstBrush;  ///< Index of the first brush in <code>brushes<code> array.
      int    nBrushes;    ///< Number of brushes.

      float  ratioInc;
      int    flags;       ///< Flags, not used for now.
      Type   type;        ///< Model type.

      float  margin;      ///< Margin around entity inside which objects trigger door opening.
      float  timeout;     ///< Timeout after which entity starts opening/closing.

      int    openSound;   ///< Open sound sample, played when an entity starts moving.
      int    closeSound;  ///< Close sound sample, played when an entity stops moving.
      int    frictSound;  ///< Friction sound sample, played while the entity is moving.
    };

    int     id;
    float   life;
    float   resistance;

    int     nPlanes;
    int     nNodes;
    int     nLeaves;
    int     nLeafBrushes;
    int     nModels;
    int     nBrushes;
    int     nBrushSides;

    Plane*  planes;
    Node*   nodes;
    Leaf*   leaves;
    int*    leafBrushes;
    Model*  models;
    Brush*  brushes;
    int*    brushSides;

    explicit BSP( int id );
    ~BSP();

};

}
}
