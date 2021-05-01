/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <matrix/BSP.hh>

#include <matrix/Liber.hh>
#include <matrix/Collider.hh>

namespace oz
{

BSP::~BSP()
{
  delete[] data;
}

BSP::BSP(const char* name_, int id_)
  : data(nullptr), name(name_), id(id_)
{
  File   file   = String::format("@bsp/%s.ozBSP", name.c());
  Stream is     = Stream(0, Endian::LITTLE);

  if (!file.read(&is)) {
    OZ_ERROR("BSP file '%s' read failed", file.c());
  }

  mins          = is.read<Point>();
  maxs          = is.read<Point>();

  title         = lingua.get(is.readString());
  description   = lingua.get(is.readString());

  life          = is.readFloat();
  resistance    = is.readFloat();

  fragPool      = liber.fragPool(is.readString());
  nFrags        = is.readInt();

  demolishSound = liber.soundIndex(is.readString());
  groundOffset  = is.readFloat();

  nPlanes       = is.readInt();
  nNodes        = is.readInt();
  nLeaves       = is.readInt();
  nLeafBrushes  = is.readInt();
  nBrushes      = is.readInt();
  nBrushSides   = is.readInt();
  nEntities     = is.readInt();
  nBoundObjects = is.readInt();

  size_t size = Alloc::alignUp(1);

  size += Alloc::alignUp(nPlanes       * sizeof(planes[0]));
  size += Alloc::alignUp(nNodes        * sizeof(nodes[0]));
  size += Alloc::alignUp(nLeaves       * sizeof(leaves[0]));
  size += Alloc::alignUp(nLeafBrushes  * sizeof(leafBrushes[0]));
  size += Alloc::alignUp(nBrushes      * sizeof(brushes[0]));
  size += Alloc::alignUp(nBrushSides   * sizeof(brushSides[0]));
  size += Alloc::alignUp(nEntities     * sizeof(entities[0]));
  size += Alloc::alignUp(nBoundObjects * sizeof(boundObjects[0]));

  OZ_ASSERT(data == nullptr);

  data = new char[size];

  char* p = Alloc::alignUp(data);

  planes = new(p) Plane[nPlanes];
  for (int i = 0; i < nPlanes; ++i) {
    planes[i] = is.read<Plane>();
  }
  p = Alloc::alignUp(p + nPlanes * sizeof(planes[0]));

  nodes = new(p) Node[nNodes];
  for (int i = 0; i < nNodes; ++i) {
    nodes[i].plane = is.readInt();
    nodes[i].front = is.readInt();
    nodes[i].back  = is.readInt();
  }
  p = Alloc::alignUp(p + nNodes * sizeof(nodes[0]));

  leaves = new(p) Leaf[nLeaves];
  for (int i = 0; i < nLeaves; ++i) {
    leaves[i].firstBrush = is.readInt();
    leaves[i].nBrushes   = is.readInt();
  }
  p = Alloc::alignUp(p + nLeaves * sizeof(leaves[0]));

  leafBrushes = new(p) int[nLeafBrushes];
  for (int i = 0; i < nLeafBrushes; ++i) {
    leafBrushes[i] = is.readInt();
  }
  p = Alloc::alignUp(p + nLeafBrushes * sizeof(leafBrushes[0]));

  brushes = new(p) Brush[nBrushes];
  for (int i = 0; i < nBrushes; ++i) {
    brushes[i].firstSide = is.readInt();
    brushes[i].nSides    = is.readInt();
    brushes[i].flags     = is.readInt();

    if (brushes[i].flags & Medium::SEA_BIT) {
//      brushes[i].flags |= orbis.terra.liquid & Medium::LIQUID_MASK;
      int liquid = orbis.terra.liquid;
      brushes[i].flags |= liquid;
    }
  }
  p = Alloc::alignUp(p + nBrushes * sizeof(brushes[0]));

  brushSides = new(p) int[nBrushSides];
  for (int i = 0; i < nBrushSides; ++i) {
    brushSides[i] = is.readInt();
  }
  p = Alloc::alignUp(p + nBrushSides * sizeof(brushSides[0]));

  entities = new(p) EntityClass[nEntities];
  for (int i = 0; i < nEntities; ++i) {
    entities[i].mins         = is.read<Point>();
    entities[i].maxs         = is.read<Point>();

    entities[i].firstBrush   = is.readInt();
    entities[i].nBrushes     = is.readInt();

    entities[i].bsp          = this;
    entities[i].title        = lingua.get(is.readString());

    entities[i].type         = EntityClass::Type(is.readInt());
    entities[i].flags        = is.readInt();

    entities[i].closeTimeout = is.readFloat();
    entities[i].openTimeout  = is.readFloat();

    entities[i].moveDir      = is.read<Vec3>();
    entities[i].moveLength   = is.readFloat();
    entities[i].moveStep     = is.readFloat();

    entities[i].margin       = is.readFloat();

    entities[i].openSound    = liber.soundIndex(is.readString());
    entities[i].closeSound   = liber.soundIndex(is.readString());
    entities[i].frictSound   = liber.soundIndex(is.readString());

    entities[i].target       = is.readInt();
    entities[i].key          = is.readInt();

    entities[i].model        = liber.modelIndex(is.readString());
    entities[i].modelTransf  = is.read<Mat4>();
  }
  p = Alloc::alignUp(p + nEntities * sizeof(entities[0]));

  boundObjects = new(p) BoundObject[nBoundObjects];
  for (int i = 0; i < nBoundObjects; ++i) {
    const ObjectClass* clazz = liber.objClass(is.readString());

    boundObjects[i].clazz   = clazz;
    boundObjects[i].pos     = is.read<Point>();
    boundObjects[i].heading = Heading(is.readInt());

    if (clazz->flags & Object::DYNAMIC_BIT) {
      OZ_ERROR("BSP '%s' bound object '%s' is not static", name.c(), clazz->name.c());
    }
  }

  OZ_ASSERT(is.available() == 0);
}

}
