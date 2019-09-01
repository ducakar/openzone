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

#include <matrix/Collider.hh>

namespace oz
{

static constexpr Vec3 NORMALS[] =
{
  Vec3(+1.0f,  0.0f,  0.0f),
  Vec3(-1.0f,  0.0f,  0.0f),
  Vec3( 0.0f, +1.0f,  0.0f),
  Vec3( 0.0f, -1.0f,  0.0f),
  Vec3( 0.0f,  0.0f, +1.0f),
  Vec3( 0.0f,  0.0f, -1.0f)
};

inline bool Collider::visitBrush(int index)
{
  bool isTested = visitedBrushes.get(index);
  visitedBrushes.set(index);
  return isTested;
}

//***********************************
//*         STATIC AABB CD          *
//***********************************

bool Collider::overlapsAABBObj(const Object* sObj) const
{
  if (flags & sObj->flags & Object::CYLINDER_BIT) {
    Vec3  relPos  = aabb.p - sObj->p;

    float sumDimXY = aabb.dim.x + sObj->dim.x;
    float sumDimZ  = aabb.dim.z + sObj->dim.z;

    float distXY2 = relPos.x*relPos.x + relPos.y*relPos.y;
    float radius  = sumDimXY + EPSILON;

    return distXY2 <= radius*radius &&
           relPos.z <= +sumDimZ + EPSILON &&
           relPos.z >= -sumDimZ - EPSILON;
  }
  else {
    return sObj->overlaps(aabb, EPSILON);
  }
}

bool Collider::overlapsAABBBrush(const BSP::Brush* brush) const
{
  bool result = true;

  for (int i = 0; i < brush->nSides; ++i) {
    const Plane& plane = bsp->planes[bsp->brushSides[brush->firstSide + i]];

    float offset = localDim * abs(plane.n);
    float dist   = startPos * plane - offset;

    result &= dist <= EPSILON;
  }
  return result;
}

bool Collider::overlapsAABBNode(int nodeIndex)
{
  if (nodeIndex < 0) {
    const BSP::Leaf& leaf = bsp->leaves[~nodeIndex];

    for (int i = 0; i < leaf.nBrushes; ++i) {
      int index = bsp->leafBrushes[leaf.firstBrush + i];
      const BSP::Brush& brush = bsp->brushes[index];

      if (!visitBrush(index) && (brush.flags & Material::STRUCT_BIT) && overlapsAABBBrush(&brush)) {
        return true;
      }
    }
    return false;
  }
  else {
    const BSP::Node& node  = bsp->nodes[nodeIndex];
    const Plane&     plane = bsp->planes[node.plane];

    float offset = localDim * abs(plane.n) + 2.0f * EPSILON;
    float dist   = startPos * plane;

    if (dist > offset) {
      return overlapsAABBNode(node.front);
    }
    else if (dist < -offset) {
      return overlapsAABBNode(node.back);
    }
    else {
      return overlapsAABBNode(node.front) || overlapsAABBNode(node.back);
    }
  }
}

bool Collider::overlapsAABBEntities()
{
  if (str->entities.isEmpty()) {
    return false;
  }

  Point  originalStartPos = startPos;
  Bounds localTrace       = str->toStructCS(trace);

  for (const Entity& e : str->entities) {
    entity = &e;

    if (localTrace.overlaps(*entity->clazz + entity->offset)) {
      for (int j = 0; j < entity->clazz->nBrushes; ++j) {
        int index = entity->clazz->firstBrush + j;
        const BSP::Brush& brush = bsp->brushes[index];

        OZ_ASSERT(!visitBrush(index));

        startPos = originalStartPos - entity->offset;

        if ((brush.flags & Material::STRUCT_BIT) && overlapsAABBBrush(&brush)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Collider::overlapsAABBOrbis()
{
  if (!orbis.includes(aabb, -EPSILON)) {
    return true;
  }

  if (aabb.p.z - aabb.dim.z - orbis.terra.getHeight(aabb.p.x, aabb.p.y) <= 0.0f) {
    return true;
  }

  visitedStructs.clear();

  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      const Cell& cell = orbis.cells[x][y];

      for (int strIndex : cell.structs) {
        str = orbis.str(strIndex);

        if (visitedStructs.get(strIndex) || !trace.overlaps(*str)) {
          continue;
        }

        visitedStructs.set(strIndex);
        visitedBrushes.clear();

        startPos = str->toStructCS(aabb.p);
        localDim = str->swapDimCS(aabb.dim);
        bsp      = str->bsp;

        if (overlapsAABBNode(0) || overlapsAABBEntities()) {
          return true;
        }
      }

      for (const Object* sObj = cell.objects.first(); sObj != nullptr; sObj = sObj->next[0]) {
        if (sObj != exclObj && (sObj->flags & mask) && overlapsAABBObj(sObj)) {
          return true;
        }
      }
    }
  }
  return false;
}

//***********************************
//*        STATIC ENTITY CD         *
//***********************************

bool Collider::overlapsEntityObjects()
{
  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      const Cell& cell = orbis.cells[x][y];

      for (const Object* sObj = cell.objects.first(); sObj != nullptr; sObj = sObj->next[0]) {
        if (trace.overlaps(*sObj)) {
          startPos = str->toStructCS(sObj->p) - entity->offset;
          localDim = str->swapDimCS(sObj->dim + Vec3(margin, margin, margin));

          for (int i = 0; i < entity->clazz->nBrushes; ++i) {
            const BSP::Brush& brush = bsp->brushes[entity->clazz->firstBrush + i];

            if ((sObj->flags & mask) && (brush.flags & Material::STRUCT_BIT) &&
                overlapsAABBBrush(&brush))
            {
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

//***********************************
//*        DYNAMIC AABB CD          *
//***********************************

void Collider::trimAABBVoid()
{
  for (int i = 0; i < 3; ++i) {
    int side = move[i] >= 0.0f;
    const Vec3& normal = NORMALS[i * 2 + side];

    float startDist = orbis.maxs[i] + startPos[i] * normal[i] - aabb.dim[i];
    float endDist   = orbis.maxs[i] + endPos[i]   * normal[i] - aabb.dim[i];

    if (endDist <= EPSILON && endDist <= startDist) {
      float ratio = startDist / max(startDist - endDist, Math::FLOAT_EPS);

      if (ratio < hit.ratio) {
        hit.ratio    = max(0.0f, ratio);
        hit.normal   = normal;
        hit.obj      = nullptr;
        hit.str      = nullptr;
        hit.entity   = nullptr;
        hit.material = Material::VOID_BIT;
      }
    }
  }
}

void Collider::trimAABBObj(const Object* sObj)
{
  float minRatio    = -1.0f;
  float maxRatio    = +1.0f;
  Vec3  lastNormal  = Vec3::ZERO;

  Vec3  relStartPos = startPos - sObj->p;
  Vec3  relEndPos   = endPos   - sObj->p;
  Vec3  sumDim      = aabb.dim + sObj->dim;

  int   firstPlane  = 0;

  if (flags & sObj->flags & Object::CYLINDER_BIT) {
    firstPlane = 2;

    float px         = relStartPos.x;
    float py         = relStartPos.y;
    float rx         = relEndPos.x;
    float ry         = relEndPos.y;
    float sx         = move.x;
    float sy         = move.y;
    float startDist2 = px*px + py*py;
    float endDist2   = rx*rx + ry*ry;
    float radius2    = sumDim.x*sumDim.x;
    float radiusEps2 = (sumDim.x + EPSILON) * (sumDim.x + EPSILON);

    if (endDist2 > radiusEps2) {
      float moveDist2    = sx*sx + sy*sy;
      float pxsx_pysy    = px*sx + py*sy;
      float pxsy_pysx    = px*sy - py*sx;
      float discriminant = radiusEps2 * moveDist2 - pxsy_pysx * pxsy_pysx;

      if (startDist2 < radius2) {
        OZ_ASSERT(discriminant >= 0.0f);

        float sqrtDiscr = Math::sqrt(discriminant);
        float endRatio  = (-pxsx_pysy + sqrtDiscr) / max(moveDist2, Math::FLOAT_EPS);

        maxRatio = min(maxRatio, endRatio);
      }
      else if (discriminant < 0.0f) {
        return;
      }
      else {
        float sqrtDiscr = Math::sqrt(discriminant);
        float endRatio  = (-pxsx_pysy + sqrtDiscr) / max(moveDist2, Math::FLOAT_EPS);

        if (endRatio <= 0.0f) {
          return;
        }

        maxRatio = min(maxRatio, endRatio);

        float startRatio = (-pxsx_pysy - sqrtDiscr) / max(moveDist2, Math::FLOAT_EPS);

        if (startRatio > minRatio) {
          minRatio = startRatio;
          lastNormal = ~Vec3(px + startRatio*sx, py + startRatio*sy, 0.0f);
        }
      }
    }
    else if (startDist2 >= radius2 && endDist2 <= startDist2) {
      float moveDist2    = sx*sx + sy*sy;
      float pxsx_pysy    = px*sx + py*sy;
      float pxsy_pysx    = px*sy - py*sx;
      float discriminant = radiusEps2 * moveDist2 - pxsy_pysx * pxsy_pysx;
      float sqrtDiscr    = Math::sqrt(max(discriminant, 0.0f));
      float startRatio   = (-pxsx_pysy - sqrtDiscr) / max(moveDist2, Math::FLOAT_EPS);

      if (startRatio > minRatio) {
        minRatio   = startRatio;
        lastNormal = ~Vec3(px + startRatio*sx, py + startRatio*sy, 0.0f);
      }
    }
  }

  for (int i = firstPlane; i < 3; ++i) {
    float startDist, endDist;

    startDist = +relStartPos[i] - sumDim[i];
    endDist   = +relEndPos[i]   - sumDim[i];

    if (endDist > EPSILON) {
      if (startDist < 0.0f) {
        maxRatio = min(maxRatio, startDist / (startDist - endDist));
      }
      else {
        return;
      }
    }
    else if (startDist >= 0.0f && endDist <= startDist) {
      float ratio = (startDist - EPSILON) / max(startDist - endDist, Math::FLOAT_EPS);

      if (ratio > minRatio) {
        minRatio   = ratio;
        lastNormal = NORMALS[2*i];
      }
    }

    startDist = -relStartPos[i] - sumDim[i];
    endDist   = -relEndPos[i]   - sumDim[i];

    if (endDist > EPSILON) {
      if (startDist < 0.0f) {
        maxRatio = min(maxRatio, startDist / (startDist - endDist));
      }
      else {
        return;
      }
    }
    else if (startDist >= 0.0f && endDist <= startDist) {
      float ratio = (startDist - EPSILON) / max(startDist - endDist, Math::FLOAT_EPS);

      if (ratio > minRatio) {
        minRatio   = ratio;
        lastNormal = NORMALS[2*i + 1];
      }
    }
  }

  if (minRatio != -1.0f && minRatio <= maxRatio && minRatio < hit.ratio) {
    hit.ratio    = max(0.0f, minRatio);
    hit.normal   = lastNormal;
    hit.obj      = const_cast<Object*>(sObj);
    hit.str      = nullptr;
    hit.entity   = nullptr;
    hit.material = Material::OBJECT_BIT;
  }
}

void Collider::trimAABBBrush(const BSP::Brush* brush)
{
  float minRatio   = -1.0f;
  float maxRatio   = +1.0f;
  Vec3  lastNormal = Vec3::ZERO;

  for (int i = 0; i < brush->nSides; ++i) {
    const Plane& plane = bsp->planes[bsp->brushSides[brush->firstSide + i]];

    float offset    = localDim * abs(plane.n);
    float startDist = startPos * plane - offset;
    float endDist   = endPos   * plane - offset;

    if (endDist > EPSILON) {
      if (startDist < 0.0f) {
        maxRatio = min(maxRatio, startDist / (startDist - endDist));
      }
      else {
        return;
      }
    }
    else if (startDist >= 0.0f && endDist <= startDist) {
      float ratio = (startDist - EPSILON) / max(startDist - endDist, Math::FLOAT_EPS);

      if (ratio > minRatio) {
        minRatio   = ratio;
        lastNormal = plane.n;
      }
    }
  }

  if (minRatio != -1.0f && minRatio <= maxRatio && minRatio < hit.ratio) {
    hit.ratio    = max(0.0f, minRatio);
    hit.normal   = str->toAbsoluteCS(lastNormal);
    hit.obj      = nullptr;
    hit.str      = const_cast<Struct*>(str);
    hit.entity   = const_cast<Entity*>(entity);
    hit.material = brush->flags & Material::MASK;
  }
}

void Collider::trimAABBLiquid(const BSP::Brush* brush)
{
  float depth = Math::INF;

  for (int i = 0; i < brush->nSides; ++i) {
    const Plane& plane = bsp->planes[bsp->brushSides[brush->firstSide + i]];

    float offset = localDim * abs(plane.n);
    float dist   = startPos * plane - offset;

    if (dist >= 0.0f) {
      return;
    }
    else if (plane.n.z > 0.0f) {
      float lowerDist = (plane.d - startPos.x*plane.n.x - startPos.y*plane.n.y) / plane.n.z -
                        startPos.z + aabb.dim.z;

      if (lowerDist > 0.0f) {
        depth = min(depth, lowerDist);
      }
      else {
        return;
      }
    }
  }

  OZ_ASSERT(0.0f < depth && depth < Math::INF);

  hit.mediumStr = const_cast<Struct*>(str);
  hit.medium   |= brush->flags & Medium::MASK;
  hit.depth     = max(hit.depth, depth);
}

void Collider::trimAABBArea(const BSP::Brush* brush)
{
  for (int i = 0; i < brush->nSides; ++i) {
    const Plane& plane = bsp->planes[bsp->brushSides[brush->firstSide + i]];

    float offset = localDim * abs(plane.n);
    float dist   = startPos * plane - offset;

    if (dist > 0.0f) {
      return;
    }
  }

  hit.mediumStr = const_cast<Struct*>(str);
  hit.medium   |= brush->flags & Medium::MASK;
}

void Collider::trimAABBNode(int nodeIndex)
{
  if (nodeIndex < 0) {
    const BSP::Leaf& leaf = bsp->leaves[~nodeIndex];

    for (int i = 0; i < leaf.nBrushes; ++i) {
      int index = bsp->leafBrushes[leaf.firstBrush + i];
      const BSP::Brush& brush = bsp->brushes[index];

      if (!visitBrush(index)) {
        if (brush.flags & Material::STRUCT_BIT) {
          trimAABBBrush(&brush);
        }
        else if (brush.flags & Medium::LIQUID_MASK) {
          trimAABBLiquid(&brush);
        }
        else {
          trimAABBArea(&brush);
        }
      }
    }
  }
  else {
    const BSP::Node& node  = bsp->nodes[nodeIndex];
    const Plane&     plane = bsp->planes[node.plane];

    float offset    = localDim * abs(plane.n) + 2.0f * EPSILON;
    float startDist = startPos * plane;
    float endDist   = endPos   * plane;

    if (startDist > offset && endDist > offset) {
      trimAABBNode(node.front);
    }
    else if (startDist < -offset && endDist < -offset) {
      trimAABBNode(node.back);
    }
    else {
      trimAABBNode(node.front);
      trimAABBNode(node.back);
    }
  }
}

void Collider::trimAABBEntities()
{
  if (str->entities.isEmpty()) {
    return;
  }

  Point  originalStartPos = startPos;
  Point  originalEndPos   = endPos;
  Bounds localTrace       = str->toStructCS(trace);

  for (const Entity& e : str->entities) {
    entity = &e;

    if (localTrace.overlaps(*entity->clazz + entity->offset)) {
      startPos = originalStartPos - entity->offset;
      endPos   = originalEndPos   - entity->offset;

      for (int j = 0; j < entity->clazz->nBrushes; ++j) {
        int index = entity->clazz->firstBrush + j;
        const BSP::Brush& brush = bsp->brushes[index];

        OZ_ASSERT(!visitBrush(index));

        trimAABBBrush(&brush);
      }
    }
  }
}

void Collider::trimAABBTerraQuad(int x, int y)
{
  const Terra::Quad& quad     = orbis.terra.quads[x][y];
  const Terra::Quad& nextQuad = orbis.terra.quads[x + 1][y + 1];

  const Point& minVert = quad.vertex;
  const Point& maxVert = nextQuad.vertex;

  Vec3 localStartPos = startPos - minVert;
  Vec3 localEndPos   = endPos   - minVert;

  float startDist = localStartPos * quad.normals[0];
  float endDist   = localEndPos   * quad.normals[0];

  if (endDist <= EPSILON && endDist <= startDist) {
    float ratio = max(0.0f, startDist - EPSILON) / max(startDist - endDist, Math::FLOAT_EPS);

    float impactX = startPos.x + ratio * move.x;
    float impactY = startPos.y + ratio * move.y;

    if (impactX - minVert.x >= impactY - minVert.y &&
        minVert.x <= impactX && impactX <= maxVert.x &&
        minVert.y <= impactY && impactY <= maxVert.y &&
        ratio < hit.ratio)
    {
      hit.ratio    = ratio;
      hit.normal   = quad.normals[0];
      hit.obj      = nullptr;
      hit.str      = nullptr;
      hit.entity   = nullptr;
      hit.material = Material::TERRAIN_BIT;

      return;
    }
  }

  startDist = localStartPos * quad.normals[1];
  endDist   = localEndPos   * quad.normals[1];

  if (endDist <= EPSILON && endDist <= startDist) {
    float ratio = max(0.0f, startDist - EPSILON) / max(startDist - endDist, Math::FLOAT_EPS);

    float impactX = startPos.x + ratio * move.x;
    float impactY = startPos.y + ratio * move.y;

    if (impactX - minVert.x <= impactY - minVert.y &&
        minVert.x <= impactX && impactX <= maxVert.x &&
        minVert.y <= impactY && impactY <= maxVert.y &&
        ratio < hit.ratio)
    {
      hit.ratio    = ratio;
      hit.normal   = quad.normals[1];
      hit.obj      = nullptr;
      hit.str      = nullptr;
      hit.entity   = nullptr;
      hit.material = Material::TERRAIN_BIT;

      return;
    }
  }
}

void Collider::trimAABBTerra()
{
  if (startPos.z < 0.0f && !(hit.medium & Medium::AIR_BIT)) {
    hit.medium |= orbis.terra.liquid;
    hit.depth   = max(hit.depth, -startPos.z);
  }

  float minPosX = min(startPos.x, endPos.x);
  float minPosY = min(startPos.y, endPos.y);
  float maxPosX = max(startPos.x, endPos.x);
  float maxPosY = max(startPos.y, endPos.y);

  Span terraSpan = orbis.terra.getInters(minPosX, minPosY, maxPosX, maxPosY);

  for (int x = terraSpan.minX; x <= terraSpan.maxX; ++x) {
    for (int y = terraSpan.minY; y <= terraSpan.maxY; ++y) {
      trimAABBTerraQuad(x, y);
    }
  }
}

void Collider::trimAABBOrbis()
{
  hit = Hit();

  Point originalStartPos = aabb.p;
  Point originalEndPos   = aabb.p + move;

  startPos = originalStartPos;
  endPos   = originalEndPos;

  if (!orbis.includes(trace)) {
    trimAABBVoid();
  }

  visitedStructs.clear();

  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      const Cell& cell = orbis.cells[x][y];

      for (int strIndex : cell.structs) {
        str = orbis.str(strIndex);

        if (visitedStructs.get(strIndex) || !trace.overlaps(*str)) {
          continue;
        }

        visitedStructs.set(strIndex);
        visitedBrushes.clear();

        startPos = str->toStructCS(originalStartPos);
        endPos   = str->toStructCS(originalEndPos);
        localDim = str->swapDimCS(aabb.dim);
        bsp      = str->bsp;
        entity   = nullptr;

        trimAABBNode(0);
        trimAABBEntities();
      }

      startPos = originalStartPos;
      endPos   = originalEndPos;

      for (const Object* sObj = cell.objects.first(); sObj != nullptr; sObj = sObj->next[0]) {
        if (sObj != exclObj && (sObj->flags & mask) && trace.overlaps(*sObj)) {
          trimAABBObj(sObj);
        }
      }
    }
  }

  startPos.z -= aabb.dim.z;
  endPos.z   -= aabb.dim.z;

  trimAABBTerra();

  OZ_ASSERT(0.0f <= hit.ratio && hit.ratio <= 1.0f);
  OZ_ASSERT(((hit.material & Material::OBJECT_BIT) != 0) == (hit.obj != nullptr));
  OZ_ASSERT(hit.depth >= 0.0f);
}

void Collider::trimEntityObjects()
{
  hit = Hit();

  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      const Cell& cell = orbis.cells[x][y];

      for (const Object* sObj = cell.objects.first(); sObj != nullptr; sObj = sObj->next[0]) {
        if ((sObj->flags & mask) && trace.overlaps(*sObj)) {
          startPos = str->toStructCS(sObj->p) - entity->offset;
          endPos   = startPos - move;

          for (int j = 0; j < entity->clazz->nBrushes; ++j) {
            int index = entity->clazz->firstBrush + j;
            const BSP::Brush& brush = bsp->brushes[index];

            trimAABBBrush(&brush);

            if (hit.str != nullptr) {
              hit.normal   = -hit.normal;
              hit.obj      = const_cast<Object*>(sObj);
              hit.str      = nullptr;
              hit.entity   = nullptr;
              hit.material = Material::OBJECT_BIT;
            }
          }
        }
      }
    }
  }

  OZ_ASSERT(0.0f <= hit.ratio && hit.ratio <= 1.0f);
  OZ_ASSERT(((hit.material & Material::OBJECT_BIT) != 0) == (hit.obj != nullptr));
  OZ_ASSERT(hit.depth >= 0.0f);
}

//***********************************
//*          OVERLAPPING            *
//***********************************

void Collider::getOrbisOverlaps(List<Struct*>* structs, List<Object*>* objects)
{
  OZ_ASSERT(structs != nullptr || objects != nullptr);

  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      const Cell& cell = orbis.cells[x][y];

      if (structs != nullptr) {
        for (int16 i : cell.structs) {
          str = orbis.str(i);

          if (trace.overlaps(*str) && !structs->contains(const_cast<Struct*>(str))) {
            visitedBrushes.clear();

            startPos = str->toStructCS(aabb.p);
            localDim = str->swapDimCS(aabb.dim);
            bsp      = str->bsp;

            if (overlapsAABBNode(0) || overlapsAABBEntities()) {
              structs->add(const_cast<Struct*>(str));
            }
          }
        }
      }

      if (objects != nullptr) {
        for (const Object* sObj = cell.objects.first(); sObj != nullptr; sObj = sObj->next[0]) {
          if ((sObj->flags & mask) && trace.overlaps(*sObj)) {
            objects->add(const_cast<Object*>(sObj));
          }
        }
      }
    }
  }
}

void Collider::getEntityOverlaps(List<Object*>* objects)
{
  OZ_ASSERT(objects != nullptr);

  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      const Cell& cell = orbis.cells[x][y];

      for (const Object* sObj = cell.objects.first(); sObj != nullptr; sObj = sObj->next[0]) {
        if ((sObj->flags & mask) && trace.overlaps(*sObj)) {
          startPos = str->toStructCS(sObj->p) - entity->offset;
          localDim = str->swapDimCS(sObj->dim + Vec3(margin, margin, margin));

          for (int i = 0; i < entity->clazz->nBrushes; ++i) {
            const BSP::Brush& brush = bsp->brushes[entity->clazz->firstBrush + i];

            if (overlapsAABBBrush(&brush)) {
              objects->add(const_cast<Object*>(sObj));
            }
          }
        }
      }
    }
  }
}

//***********************************
//*            PUBLIC               *
//***********************************

bool Collider::overlaps(const Point& point, const Object* exclObj_)
{
  aabb    = AABB(point, Vec3::ZERO);
  exclObj = exclObj_;
  flags   = Object::CYLINDER_BIT;

  span    = orbis.getInters(point, Object::MAX_DIM);

  return overlapsAABBOrbis();
}

bool Collider::overlaps(const AABB& aabb_, const Object* exclObj_)
{
  aabb    = aabb_;
  exclObj = exclObj_;
  flags   = 0;

  trace   = Bounds(aabb, 2.0f * EPSILON);
  span    = orbis.getInters(trace, Object::MAX_DIM);

  return overlapsAABBOrbis();
}

bool Collider::overlaps(const Object* obj_)
{
  aabb    = *obj_;
  exclObj = obj_;
  flags   = obj_->flags;

  trace   = Bounds(aabb, 2.0f * EPSILON);
  span    = orbis.getInters(trace, Object::MAX_DIM);

  return overlapsAABBOrbis();
}

bool Collider::overlaps(const Entity* entity_, float margin_)
{
  str    = entity_->str;
  entity = entity_;
  bsp    = entity_->clazz->bsp;
  margin = margin_;

  trace  = Bounds(str->toAbsoluteCS(*entity->clazz + entity->offset), 2.0f * EPSILON + margin);
  span   = orbis.getInters(trace, Object::MAX_DIM);

  return overlapsEntityObjects();
}

void Collider::translate(const Point& point, const Vec3& move_, const Object* exclObj_)
{
  aabb    = AABB(point, Vec3::ZERO);
  move    = move_;
  exclObj = exclObj_;
  flags   = Object::CYLINDER_BIT;

  trace   = Bounds(point, 2.0f * EPSILON).expand(move);
  span    = orbis.getInters(trace, Object::MAX_DIM);

  trimAABBOrbis();
}

void Collider::translate(const AABB& aabb_, const Vec3& move_, const Object* exclObj_)
{
  obj     = nullptr;
  aabb    = aabb_;
  move    = move_;
  exclObj = exclObj_;
  flags   = 0;

  trace   = Bounds(aabb, 2.0f * EPSILON).expand(move);
  span    = orbis.getInters(trace, Object::MAX_DIM);

  trimAABBOrbis();
}

void Collider::translate(const Dynamic* obj_, const Vec3& move_)
{
  OZ_ASSERT(obj_->cell != nullptr);

  obj     = obj_;
  aabb    = *obj_;
  move    = move_;
  exclObj = obj_;
  flags   = obj_->flags;

  trace   = Bounds(aabb, 2.0f * EPSILON).expand(move);
  span    = orbis.getInters(trace, Object::MAX_DIM);

  trimAABBOrbis();
}

void Collider::translate(const Entity* entity_, const Vec3& localMove)
{
  str    = entity_->str;
  entity = entity_;
  bsp    = entity_->clazz->bsp;
  move   = localMove;
  flags  = 0;

  trace  = str->toAbsoluteCS(Bounds(*entity->clazz + entity->offset, 2.0f * EPSILON).expand(move));
  span   = orbis.getInters(trace, Object::MAX_DIM);

  trimEntityObjects();
}

void Collider::getOverlaps(const AABB& aabb_, List<Struct*>* structs, List<Object*>* objects,
                           float margin_)
{
  aabb    = AABB(aabb_, margin_);

  trace   = Bounds(aabb, 0.0f);
  span    = orbis.getInters(trace, Object::MAX_DIM);

  getOrbisOverlaps(structs, objects);
}

void Collider::getOverlaps(const Entity* entity_, List<Object*>* objects, float margin_)
{
  str    = entity_->str;
  entity = entity_;
  bsp    = entity_->clazz->bsp;
  margin = margin_;

  trace  = Bounds(str->toAbsoluteCS(*entity->clazz + entity->offset), margin);
  span   = orbis.getInters(trace, Object::MAX_DIM);

  getEntityOverlaps(objects);
}

Collider collider;

}
