/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include <matrix/ObjectClass.hh>

namespace oz
{

class BSP;

/**
 * %BSP model (doors, lifts etc.).
 */
struct EntityClass : Bounds
{
  enum Type
  {
    STATIC,
    MOVER,
    DOOR,
    PORTAL
  };

  /// Don't check for collisions with objects when moving, just go through objects.
  static constexpr int IGNORANT = 0x01;

  /// Push obstacles on the way when moving.
  static constexpr int PUSHER = 0x02;

  /// If pushing obstacles fails, crush them.
  static constexpr int CRUSHER = 0x04;

  /// Revert motion instead of stopping when an obstacle is detected (and cannot be pushed).
  static constexpr int REVERTER = 0x08;

  /// Automatically open (after openTimeout) when an object overlaps with the margin.
  static constexpr int AUTO_OPEN = 0x10;

  /// Automatically close (after closeTimeout) when an object overlaps with the margin.
  static constexpr int AUTO_CLOSE = 0x20;

  int    firstBrush;   ///< Index of the first brush in `brushes` array.
  int    nBrushes;     ///< Number of brushes.

  BSP*   bsp;          ///< Pointer to the parent %BSP.
  String title;        ///< %Entity title.

  Type   type;         ///< %Entity type.
  int    flags;

  float  closeTimeout; ///< Timeout after which entity starts opening.
  float  openTimeout;  ///< Timeout after which entity starts closing.

  Vec3   moveDir;      ///< Move direction vector in %BSP's CS.
  float  moveLength;   ///< Length of move vector.
  float  moveStep;     ///< Move step distance in each frame.

  float  margin;       ///< Margin around entity inside which door opening is triggered.

  int    openSound;    ///< Open sound sample, played when an entity starts moving or - for static
                       ///< entities - when activated (as a trigger not as a target).
  int    closeSound;   ///< Close sound sample, played when an entity stops moving.
  int    frictSound;   ///< Friction sound sample, played while the entity is moving.

  int    target;       ///< Target entity's index in the same BSP for triggers, -1 otherwise.
  int    key;          ///< Default key code or 0 if door is unlocked by default.

  int    model;        ///< %Model index, -1 if none.
  Mat4   modelTransf;  ///< %Model transformation.
};

/**
 * %BSP structure.
 */
class BSP : public Bounds
{
public:

  static constexpr int MAX_BRUSHES = 1024;

  /**
   * %BSP node.
   */
  struct Node
  {
    int plane; ///< Separating plane index in `planes` array.

    int front; ///< Index of node on the positive side of the separating plane.
    int back;  ///< Index of node on the negative side of the separating plane.
  };

  /**
   * %BSP leaf node.
   */
  struct Leaf
  {
    int firstBrush; ///< Index of the first brush index in `leafBrushes` array.
    int nBrushes;   ///< Number of brush indices.
  };

  /**
   * %BSP brush (convex polytope).
   */
  struct Brush
  {
    int firstSide; ///< Index of the first plane index in `bushSides` array.
    int nSides;    ///< Number of plane indices.

    int flags;     ///< %Material and medium bits (look `matrix::Material` and `matrix::Medium`).
  };

  struct BoundObject
  {
    const ObjectClass* clazz;
    Point              pos;
    Heading            heading;
  };

  char*           data;

  Plane*          planes;
  Node*           nodes;
  Leaf*           leaves;
  int*            leafBrushes;
  EntityClass*    entities;
  Brush*          brushes;
  int*            brushSides;
  BoundObject*    boundObjects;

  int             nPlanes;
  int             nNodes;
  int             nLeaves;
  int             nLeafBrushes;
  int             nEntities;
  int             nBrushes;
  int             nBrushSides;
  int             nBoundObjects;

  String          name;          ///< Name.
  String          title;         ///< Title.
  String          description;   ///< Description.

  float           life;
  float           resistance;

  const FragPool* fragPool;
  int             nFrags;

  int             demolishSound;
  float           groundOffset;  ///< Centre offset from ground when placing a building.

  int             id;            ///< Used for indexing BSPs in Context.

public:

  explicit BSP(const char* name_, int id_);

  void load();
  void unload();

};

}
