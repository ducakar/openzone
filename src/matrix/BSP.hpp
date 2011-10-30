/*
 *  BSP.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file matrix/BSP.hpp
 */

#include "stable.hpp"

#include "matrix/common.hpp"

namespace oz
{

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

        int material;  ///< Material bits (look <code>oz::::Material</code>.
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

        int    openSample;  ///< Open sound sample, played when an entity starts moving.
        int    closeSample; ///< Close sound sample, played when an entity stops moving.
        int    frictSample; ///< Friction sound sample, played while the entity is moving.
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
