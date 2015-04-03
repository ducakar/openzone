/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file client/ui/StrategicArea.cc
 */

#include <client/ui/StrategicArea.hh>

#include <client/Input.hh>
#include <client/Shape.hh>
#include <client/Camera.hh>
#include <client/ui/Mouse.hh>
#include <client/ui/Bar.hh>

namespace oz
{
namespace client
{
namespace ui
{

const float StrategicArea::TAG_REACH_DIST     = 100.0f;
const float StrategicArea::TAG_CLIP_DIST      = 0.1f;
const float StrategicArea::TAG_CLIP_K         = 9.0f;
const float StrategicArea::TAG_MIN_PIXEL_SIZE = 4.0f; // size in pixels
const float StrategicArea::TAG_MAX_COEFF_SIZE = 4.0f; // size in coefficient

bool StrategicArea::projectPoint(const Point& p, int* x, int* y) const
{
  Vec3  t  = camera.rotTMat * (p - camera.p);
  float d  = -t.z;    if (d < TAG_CLIP_DIST)                    { return false; }
  float kx = t.x / d; if (kx < -TAG_CLIP_K || +TAG_CLIP_K < kx) { return false; }
  float ky = t.y / d; if (ky < -TAG_CLIP_K || +TAG_CLIP_K < ky) { return false; }

  *x = camera.centreX + Math::lround(kx * stepPixel);
  *y = camera.centreY + Math::lround(ky * stepPixel);
  return true;
}

bool StrategicArea::projectBounds(const AABB& bb, Span* span) const
{
  Vec3 p = bb.p - camera.p;
  Vec3 corners[8] = {
    p + Vec3(-bb.dim.x, -bb.dim.y, -bb.dim.z),
    p + Vec3(-bb.dim.x, -bb.dim.y, +bb.dim.z),
    p + Vec3(-bb.dim.x, +bb.dim.y, -bb.dim.z),
    p + Vec3(-bb.dim.x, +bb.dim.y, +bb.dim.z),
    p + Vec3(+bb.dim.x, -bb.dim.y, -bb.dim.z),
    p + Vec3(+bb.dim.x, -bb.dim.y, +bb.dim.z),
    p + Vec3(+bb.dim.x, +bb.dim.y, -bb.dim.z),
    p + Vec3(+bb.dim.x, +bb.dim.y, +bb.dim.z)
  };

  float minX = +Math::INF;
  float minY = +Math::INF;
  float maxX = -Math::INF;
  float maxY = -Math::INF;

  for (int i = 0; i < 8; ++i) {
    Vec3  t = camera.rotTMat * corners[i];
    float d  = -t.z;    if (d < TAG_CLIP_DIST)                    { return false; }
    float kx = t.x / d; if (kx < -TAG_CLIP_K || +TAG_CLIP_K < kx) { return false; }
    float ky = t.y / d; if (ky < -TAG_CLIP_K || +TAG_CLIP_K < ky) { return false; }

    float x = kx * stepPixel;
    float y = ky * stepPixel;

    minX = min(minX, x);
    minY = min(minY, y);
    maxX = max(maxX, x);
    maxY = max(maxY, y);
  }

  if (maxX - minX < TAG_MIN_PIXEL_SIZE || (maxX - minX) * pixelStep > TAG_MAX_COEFF_SIZE ||
      maxY - minY < TAG_MIN_PIXEL_SIZE || (maxY - minY) * pixelStep > TAG_MAX_COEFF_SIZE)
  {
    return false;
  }

  span->minX = camera.centreX + Math::lround(minX);
  span->minY = camera.centreY + Math::lround(minY);
  span->maxX = camera.centreX + Math::lround(maxX);
  span->maxY = camera.centreY + Math::lround(maxY);

  return true;
}

Vec3 StrategicArea::getRay(int x, int y)
{
  float cx = float(x - camera.centreX) * pixelStep;
  float cy = float(y - camera.centreY) * pixelStep;
  Vec3  at = Vec3(cx * TAG_REACH_DIST, cy * TAG_REACH_DIST, -TAG_REACH_DIST);

  return camera.rotMat * at;
}

void StrategicArea::collectHovers()
{
  bool isDragging = dragStartX >= 0 && mouse.x != dragStartX && mouse.y != dragStartY;

  if (!isDragging) {
    Vec3 reach = getRay(mouse.x, mouse.y);

    collider.translate(camera.p, reach);

    const Struct* str = collider.hit.str;
    const Entity* ent = collider.hit.entity;
    const Object* obj = collider.hit.obj;

    if (str != nullptr) {
      hoverStr = str->index;

      if (ent != nullptr) {
        hoverEnt = ent->index();
      }
    }
    else if (obj != nullptr) {
      hoverObj = obj->index;
    }
  }
  else {
    Span drag = {
      min(mouse.x, dragStartX),
      min(mouse.y, dragStartY),
      max(mouse.x, dragStartX),
      max(mouse.y, dragStartY)
    };

    Vec3 rays[] = {
      getRay(dragStartX, dragStartY),
      getRay(mouse.x,    dragStartY),
      getRay(dragStartX, mouse.y   ),
      getRay(mouse.x,    mouse.y   )
    };

    float minX = +Math::INF;
    float minY = +Math::INF;
    float maxX = -Math::INF;
    float maxY = -Math::INF;

    for (int i = 0; i < 4; ++i) {
      collider.translate(camera.p, rays[i]);
      Point point = camera.p + collider.hit.ratio * rays[i];

      minX = min(minX, point.x);
      minY = min(minY, point.y);
      maxX = max(maxX, point.x);
      maxY = max(maxY, point.y);
    }

    Span span = orbis.getInters(minX, minY, maxX, maxY);

    for (int x = span.minX; x <= span.maxX; ++x) {
      for (int y = span.minY; y <= span.maxY; ++y) {
        const Cell& cell = orbis.cells[x][y];

        for (const Object& obj : cell.objects) {
          if ((obj.flags & Object::SOLID_BIT) &&
              (obj.flags & (Object::BOT_BIT | Object::VEHICLE_BIT)))
          {
            int  projX, projY;
            bool hasProj = projectPoint(obj.p, &projX, &projY);

            if (hasProj && drag.minX <= projX && projX <= drag.maxX &&
                drag.minY <= projY && projY <= drag.maxY)
            {
              dragObjs.include(obj.index);
            }
          }
        }
      }
    }
  }
}

void StrategicArea::drawHoverTitle(const Span& span, const char* title)
{
  int labelX = (span.minX + span.maxX) / 2;
  int labelY = span.maxY + 16;

  unitName.setPosition(labelX, labelY);
  unitName.setText("%s", title);
  unitName.draw(this);
}

void StrategicArea::drawHoverRect(const Span& span, const Struct* str, const Object* obj)
{
  float life;
  float status   = -1.0f;
  int   barWidth = span.maxX - span.minX + 4;

  if (str != nullptr) {
    life = str->life / str->bsp->life;
  }
  else {
    const ObjectClass* clazz = obj->clazz;

    life   = obj->flags & Object::BOT_BIT ? (2.0f * obj->life - clazz->life) / clazz->life :
                                            obj->life / clazz->life;
    status = obj->status();
  }

  if (status < 0.0f) {
    Bar lifeBar(&style.hoverLife);
    lifeBar.drawAbs(span.minX - 2, span.maxY + 2, barWidth, 8, life);
  }
  else {
    Bar lifeBar(&style.hoverLife);
    Bar statusBar(&style.hoverStatus);

    lifeBar.drawAbs(span.minX - 2, span.maxY + 7, barWidth, 8, life);
    statusBar.drawAbs(span.minX - 2, span.maxY + 2, barWidth, 6, status);
  }
}

void StrategicArea::drawTagRect(const Span& span, const Struct* str, const Object* obj,
                                bool isHovered)
{
  float minX = float(span.minX);
  float maxX = float(span.maxX);
  float minY = float(span.minY);
  float maxY = float(span.maxY);

  if (!isHovered) {
    float life   = +1.0f;
    float status = -1.0f;

    if (str != nullptr) {
      float maxLife = str->bsp->life;

      life = str->life / maxLife;
    }
    else {
      float maxLife = obj->clazz->life;

      life   = obj->flags & Object::BOT_BIT ? (2.0f * obj->life - maxLife) / maxLife :
                                              obj->life / maxLife;
      status = obj->status();
    }

    int barWidth = span.maxX - span.minX + 4;

    if (status < 0.0f) {
      Bar lifeBar(&style.selectedLife);

      lifeBar.drawAbs(span.minX - 2, span.maxY + 2, barWidth, 8, life);
    }
    else {
      Bar lifeBar(&style.selectedLife);
      Bar statusBar(&style.selectedStatus);

      lifeBar.drawAbs(span.minX - 2, span.maxY + 7, barWidth, 8, life);
      statusBar.drawAbs(span.minX - 2, span.maxY + 2, barWidth, 6, status);
    }
  }

  shape.colour(style.selectedLife.border);
  shape.tag(minX, minY, maxX, maxY);
}

void StrategicArea::onVisibilityChange(bool)
{
  clearOverlay();

  dragStartX = -1;
  dragStartY = -1;

  hoverStr   = -1;
  hoverObj   = -1;
  hoverEnt   = -1;
  dragObjs.clear();

  taggedStr  = -1;
  taggedObjs.clear();

  mouseW     = 0.0f;
}

void StrategicArea::onRealign()
{
  width     = camera.width;
  height    = camera.height;

  pixelStep = camera.coeff / float(camera.height / 2);
  stepPixel = 1.0f / pixelStep;
}

void StrategicArea::onUpdate()
{
  if (!mouse.isVisible) {
    clearOverlay();

    dragStartX = -1;
    dragStartY = -1;
    mouseW     = input.mouseW * input.mouseSensW;
    return;
  }

  if (!((input.buttons | input.oldButtons) & Input::LEFT_BUTTON)) {
    dragStartX = -1;
    dragStartY = -1;
  }

  taggedStr = orbis.strIndex(taggedStr);

  for (int i = 0; i < taggedObjs.length();) {
    Object* obj = orbis.obj(taggedObjs[i]);

    if (obj == nullptr || obj->cell == nullptr) {
      taggedObjs.erase(i);
    }
    else {
      ++i;

      if (taggedObjs.length() == 1) {
        camera.setTaggedObj(obj);
      }
    }
  }

  if (input.rightPressed) {
    clearOverlay();
  }
}

bool StrategicArea::onMouseEvent()
{
  mouseW = input.mouseW * input.mouseSensW;

  if (overlayCallback != nullptr) {
    Vec3 ray = getRay(mouse.x, mouse.y);

    overlayCallback(overlaySender, ray);
  }
  else {
    collectHovers();

    if (input.leftPressed) {
      dragStartX = mouse.x;
      dragStartY = mouse.y;

      if (!input.keys[Input::KEY_GROUP_SELECT]) {
        taggedStr = -1;
        taggedObjs.clear();
      }
    }
    else if (!(input.buttons & Input::LEFT_BUTTON)) {
      dragStartX = -1;
      dragStartY = -1;

      if (input.oldButtons & Input::LEFT_BUTTON) {
        taggedStr = taggedObjs.isEmpty() ? hoverStr : -1;

        if (hoverObj != -1) {
          if (taggedObjs.contains(hoverObj)) {
            taggedObjs.exclude(hoverObj);
          }
          else {
            taggedObjs.include(hoverObj);
          }
        }

        for (int obj : dragObjs) {
          taggedObjs.include(obj);
        }
      }
    }
  }
  return true;
}

void StrategicArea::onDraw()
{
  const Struct* str = orbis.str(hoverStr);
  const Entity* ent = orbis.ent(hoverEnt);
  const Object* obj = orbis.obj(hoverObj);

  Span span;

  if (str != nullptr) {
    if (projectBounds(str->toAABB(), &span)) {
      drawHoverRect(span, str, nullptr);

      if (ent == nullptr) {
        drawHoverTitle(span, str->bsp->title);
      }
    }
    if (ent != nullptr &&
        projectBounds(str->toAbsoluteCS(*ent->clazz + ent->offset).toAABB(), &span))
    {
      drawHoverTitle(span, ent->clazz->title);
    }
  }

  if (obj != nullptr && obj->cell != nullptr && projectBounds(*obj, &span)) {
    drawHoverRect(span, nullptr, obj);
    drawHoverTitle(span, obj->title());
  }

  for (int i = 0; i < dragObjs.length(); ++i) {
    obj = orbis.obj(dragObjs[i]);

    if (obj != nullptr && obj->cell != nullptr && projectBounds(*obj, &span)) {
      drawHoverRect(span, nullptr, obj);
    }
  }

  str = orbis.str(taggedStr);

  if (str != nullptr && (str->p - camera.p) * camera.at >= TAG_CLIP_DIST &&
      projectBounds(str->toAABB(), &span))
  {
    drawTagRect(span, str, nullptr, str->index == hoverStr);
  }

  for (int i = 0; i < taggedObjs.length(); ++i) {
    obj = orbis.obj(taggedObjs[i]);

    if (obj != nullptr && obj->cell != nullptr &&
        (obj->p - camera.p) * camera.at >= TAG_CLIP_DIST && projectBounds(*obj, &span))
    {
      drawTagRect(span, nullptr, obj, obj->index == hoverObj || dragObjs.contains(obj->index));
    }
  }

  if (dragStartX >= 0) {
    int width  = mouse.x - dragStartX;
    int height = mouse.y - dragStartY;

    if (abs(width) > 2 || abs(height) > 2) {
      shape.colour(1.0f, 1.0f, 1.0f, 1.0f);
      shape.rect(dragStartX, dragStartY, width, height);
    }
  }

  hoverStr = -1;
  hoverEnt = -1;
  hoverObj = -1;
  dragObjs.clear();
}

StrategicArea::StrategicArea() :
  Area(camera.width, camera.height),
  unitName(0, 0, 0, ALIGN_HCENTRE, Font::SANS, ""),
  overlayCallback(nullptr), overlaySender(nullptr),
  hoverStr(-1), hoverEnt(-1), hoverObj(-1), taggedStr(-1),
  mouseW(0.0f)
{
  flags |= UPDATE_BIT | PINNED_BIT;
}

void StrategicArea::setOverlay(OverlayCallback* callback, Area* sender)
{
  overlayCallback = callback;
  overlaySender   = sender;
}

void StrategicArea::clearOverlay()
{
  overlayCallback = nullptr;
  overlaySender   = nullptr;
}

}
}
}
