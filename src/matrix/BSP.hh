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
 * @file matrix/BSP.hh
 */

#pragma once

#include "matrix/ObjectClass.hh"

namespace oz
{
namespace matrix
{

class BSP;

/**
 * %BSP model (doors, lifts etc.).
 *
 * @ingroup matrix
 */
struct Model : Bounds
{
  enum Type
  {
    STATIC,
    MANUAL_DOOR,
    AUTO_DOOR,
    IGNORING_BLOCK,
    CRUSHING_BLOCK,
    ELEVATOR
  };

  String name;       ///< Model name.
  String title;      ///< Model title.

  Vec3   move;       ///< Move vector (destination - original position), in %BSP
                     ///< coordinate system.

  BSP*   bsp;        ///< Pointer to the parent %BSP.

  int    firstBrush; ///< Index of the first brush in <tt>brushes</tt> array.
  int    nBrushes;   ///< Number of brushes.

  Type   type;       ///< Model type.
  float  margin;     ///< Margin around entity inside which triggers door opening.
  float  timeout;    ///< Timeout after which entity starts opening/closing.
  float  ratioInc;   ///< Step in ratio for each frame.

  int    target;     ///< Target model index for triggers, -1 otherwise.
  int    key;        ///< Default key code or 0 if door is unlocked by default.

  int    openSound;  ///< Open sound sample, played when an entity starts moving or - for static
                     ///< entities - when activated (as a trigger not as a target).
  int    closeSound; ///< Close sound sample, played when an entity stops moving.
  int    frictSound; ///< Friction sound sample, played while the entity is moving.
};

/**
 * %BSP structure.
 *
 * @ingroup matrix
 */
class BSP : public Bounds
{
  public:

    static const int MAX_BRUSHES = 1024;

    /**
     * %BSP node.
     */
    struct Node
    {
      int plane; ///< Separating plane index in <tt>planes</tt> array.

      int front; ///< Index of node on the positive side of the separating plane.
      int back;  ///< Index of node on the negative side of the separating plane.
    };

    /**
     * %BSP leaf node.
     */
    struct Leaf
    {
      int firstBrush; ///< Index of the first brush index in <tt>leafBrushes</tt> array.
      int nBrushes;   ///< Number of brush indices.
    };

    /**
     * %BSP brush (convex polytope).
     */
    struct Brush
    {
      int firstSide; ///< Index of the first plane index in <tt>bushSides</tt> array.
      int nSides;    ///< Number of plane indices.

      int flags;     ///< %Material and medium bits (look <tt>matrix::Material</tt> and
                     ///< <tt>matrix::Medium</tt>).
    };

    struct BoundObject
    {
      const ObjectClass* clazz;
      Point3             pos;
      Heading            heading;
    };

    String          name;        ///< Name.
    String          title;       ///< Title.
    String          description; ///< Description.
    Vector<int>     sounds;      ///< Set of used sound samples.

    float           life;
    float           resistance;

    int             nPlanes;
    int             nNodes;
    int             nLeaves;
    int             nLeafBrushes;
    int             nModels;
    int             nBrushes;
    int             nBrushSides;
    int             nBoundObjects;

    Plane*          planes;
    Node*           nodes;
    Leaf*           leaves;
    int*            leafBrushes;
    Model*          models;
    Brush*          brushes;
    int*            brushSides;
    BoundObject*    boundObjects;

    const FragPool* fragPool;
    int             nFrags;

    int             id;            ///< Used for indexing BSPs in Context.
    int             nUsers;

    explicit BSP( const char* name, int id );
    ~BSP();

    void request();
    void release();

    void load();
    void unload();

    void init();

};

inline void BSP::request()
{
  hard_assert( !name.isEmpty() );

  if( nUsers == 0 ) {
    load();
  }

  ++nUsers;
}

inline void BSP::release()
{
  hard_assert( nUsers > 0 );

  --nUsers;

  if( nUsers == 0 ) {
    unload();
  }
}

}
}

/**
 * @page BSP Structures (BSPs)
 *
 * @section BSPModel Structure Entities (BSP Models)
 */
