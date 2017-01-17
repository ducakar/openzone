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

#include <client/ui/BuildFrame.hh>

#include <matrix/Synapse.hh>
#include <nirvana/TechGraph.hh>
#include <client/Input.hh>
#include <client/Shape.hh>
#include <client/Shader.hh>
#include <client/Camera.hh>
#include <client/Context.hh>
#include <client/EditStage.hh>
#include <client/ui/StrategicArea.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{
namespace ui
{

const Mat4 BuildFrame::OVERLAY_GREEN  = Mat4(0.0f, 1.0f, 0.0f, 0.0f,
                                             0.0f, 1.0f, 0.0f, 0.0f,
                                             0.0f, 1.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 0.5f);

const Mat4 BuildFrame::OVERLAY_YELLOW = Mat4(0.8f, 0.8f, 0.0f, 0.0f,
                                             0.8f, 0.8f, 0.0f, 0.0f,
                                             0.8f, 0.8f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 0.5f);

const Mat4 BuildFrame::OVERLAY_RED    = Mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                             1.0f, 0.0f, 0.0f, 0.0f,
                                             1.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 0.5f);

void BuildFrame::overlayCallback(Area* area, const Vec3& ray)
{
  BuildFrame*        buildFrame = static_cast<BuildFrame*>(area);
  const BSP*         bsp        = buildFrame->overlayBSP;
  const ObjectClass* clazz      = buildFrame->overlayClass;
  Heading            heading    = buildFrame->overlayHeading;
  bool               overlaps   = false;

  collider.translate(camera.p, ray);

  Point position = camera.p + collider.hit.ratio * ray;
  position.z += bsp != nullptr ? bsp->groundOffset : clazz->dim.z;
  position.z += 4.0f * EPSILON;

  tf.model = Mat4::translation(position - Point::ORIGIN);
  tf.model.rotateZ(float(heading) * Math::TAU / 4.0f);

  if (buildFrame->overlayBSP != nullptr) {
    Bounds    bounds   = rotate(*bsp, heading) + (position - Point::ORIGIN);
    BSPImago* bspModel = context.requestBSP(bsp);

    List<Struct*> strs;
    List<Object*> objs;

    collider.getOverlaps(bounds.toAABB(), &strs, &objs, 2.0f * EPSILON);
    overlaps  = !strs.isEmpty() || !objs.isEmpty();
    tf.colour = overlaps ? OVERLAY_RED : OVERLAY_GREEN;

    if (!overlaps) {
      // Check if ground is plain enough.
      float corners[][2] = {
        {bounds.mins.x, bounds.mins.y},
        {bounds.maxs.x, bounds.mins.y},
        {bounds.mins.x, bounds.maxs.y},
        {bounds.maxs.x, bounds.maxs.y}
      };

      for (int i = 0; i < 4; ++i) {
        if (orbis.terra.getHeight(corners[i][0], corners[i][1]) < bounds.mins.z) {
          overlaps  = true;
          tf.colour = OVERLAY_YELLOW;
          break;
        }
      }
    }

    bspModel->schedule(nullptr, Model::OVERLAY_QUEUE);
  }
  else {
    Vec3   dim   = clazz->dim + Vec3(2.0f*EPSILON, 2.0f*EPSILON, 2.0f*EPSILON);
    AABB   bb    = AABB(position, rotate(dim, heading));
    Model* model = context.requestModel(clazz->imagoModel);

    overlaps  = collider.overlaps(bb);
    tf.colour = overlaps ? OVERLAY_RED : OVERLAY_GREEN;

    model->schedule(0, Model::OVERLAY_QUEUE);
    context.releaseModel(clazz->imagoModel);
  }

  tf.colour = Mat4::ID;

  if (input.leftPressed) {
    buildFrame->overlayHeading = Heading((heading + 1) % 4);
  }
  else if (input.middlePressed && !overlaps) {
    if (bsp != nullptr) {
      synapse.add(bsp, position, heading, false);
    }
    else {
      synapse.add(clazz, position, heading, false);
    }

    buildFrame->overlayBSP     = nullptr;
    buildFrame->overlayClass   = nullptr;
    buildFrame->overlayHeading = NORTH;

    if (ui.strategicArea) {
      ui.strategicArea->clearOverlay();
    }
  }
}

void BuildFrame::selectBuildings(Button* sender)
{
  BuildFrame* buildFrame = static_cast<BuildFrame*>(sender->parent);

  buildFrame->mode = BUILDINGS;
  buildFrame->title.setText("%s", OZ_GETTEXT("Buildings"));
}

void BuildFrame::selectUnits(Button* sender)
{
  BuildFrame* buildFrame = static_cast<BuildFrame*>(sender->parent);

  buildFrame->mode = UNITS;
  buildFrame->title.setText("%s", OZ_GETTEXT("Units"));
}

void BuildFrame::selectItems(Button* sender)
{
  BuildFrame* buildFrame = static_cast<BuildFrame*>(sender->parent);

  buildFrame->mode = ITEMS;
  buildFrame->title.setText("%s", OZ_GETTEXT("Items"));
}

void BuildFrame::selectObjects(Button* sender)
{
  BuildFrame* buildFrame = static_cast<BuildFrame*>(sender->parent);

  buildFrame->mode = OBJECTS;
  buildFrame->title.setText("%s", OZ_GETTEXT("Miscellaneous"));
}

void BuildFrame::startPlacement(ModelField* sender, bool isClicked)
{
  if (sender->id == -1) {
    return;
  }

  BuildFrame* buildFrame = static_cast<BuildFrame*>(sender->parent);

  buildFrame->isOverModel  = true;
  buildFrame->wasOverModel = true;

  if (buildFrame->mode == BUILDINGS) {
    const BSP* bsp = techGraph.allowedBuildings[sender->id];

    buildFrame->title.setText("%s", bsp->title.c());

    if (isClicked && ui.strategicArea != nullptr) {
      buildFrame->overlayBSP     = bsp;
      buildFrame->overlayClass   = nullptr;
      buildFrame->overlayHeading = NORTH;

      ui.strategicArea->setOverlay(overlayCallback, buildFrame);
    }
  }
  else {
    const ObjectClass* clazz = buildFrame->mode == UNITS ? techGraph.allowedUnits[sender->id] :
                               buildFrame->mode == ITEMS ? techGraph.allowedItems[sender->id] :
                                                           techGraph.allowedObjects[sender->id];

    buildFrame->title.setText("%s", clazz->title.c());

    if (isClicked && ui.strategicArea != nullptr) {
      buildFrame->overlayBSP     = nullptr;
      buildFrame->overlayClass   = clazz;
      buildFrame->overlayHeading = NORTH;

      Object* container = camera.objectObj;

      if (buildFrame->mode == ITEMS && container != nullptr && container->clazz->nItems != 0 &&
          editStage.editFrame != nullptr && editStage.editFrame->isVisible())
      {
        if (container->items.size() != container->clazz->nItems) {
          Heading  heading = Heading(Math::rand(4));
          Object*  newObj  = synapse.add(clazz, Point::ORIGIN, heading, false);
          Dynamic* newItem = static_cast<Dynamic*>(newObj);

          newItem->parent = container->index;
          container->items.add(newItem->index);

          synapse.cut(newItem);
        }
      }
      else {
        ui.strategicArea->setOverlay(overlayCallback, buildFrame);
      }
    }
  }
}

void BuildFrame::onRealign()
{
  children.free();
  delete[] models;

  rows   = max(0, (camera.height - 320) / SLOT_SIZE);
  height = HEADER_SIZE + 58 + rows * SLOT_SIZE;

  Pos2 pos = parent->alignChild(defaultX, defaultY, width, height);

  x = pos.x;
  y = pos.y;

  if (rows != 0) {
    add(new Button(OZ_GETTEXT("B"), selectBuildings, 55, 18),   4, -HEADER_SIZE - 2);
    add(new Button(OZ_GETTEXT("U"), selectUnits,     55, 18),  63, -HEADER_SIZE - 2);
    add(new Button(OZ_GETTEXT("I"), selectItems,     55, 18), 122, -HEADER_SIZE - 2);
    add(new Button(OZ_GETTEXT("M"), selectObjects,   55, 18), 181, -HEADER_SIZE - 2);

    models = new ModelField*[rows * 3] {};

    for (int i = 0; i < rows; ++i) {
      models[i*3 + 0] = new ModelField(startPlacement, SLOT_SIZE);
      models[i*3 + 1] = new ModelField(startPlacement, SLOT_SIZE);
      models[i*3 + 2] = new ModelField(startPlacement, SLOT_SIZE);

      models[i*3 + 0]->id = i*3 + 0;
      models[i*3 + 1]->id = i*3 + 1;
      models[i*3 + 2]->id = i*3 + 2;

      models[i*3 + 0]->show(false);
      models[i*3 + 1]->show(false);
      models[i*3 + 2]->show(false);

      add(models[i*3 + 0],   3, -HEADER_SIZE - 40 - i * SLOT_SIZE);
      add(models[i*3 + 1],  81, -HEADER_SIZE - 40 - i * SLOT_SIZE);
      add(models[i*3 + 2], 159, -HEADER_SIZE - 40 - i * SLOT_SIZE);
    }
  }
}

bool BuildFrame::onMouseEvent()
{
  Frame::onMouseEvent();

  if (input.wheelDown) {
    scroll = clamp(scroll + 1, 0, nScrollRows);
  }
  else if (input.wheelUp) {
    scroll = clamp(scroll - 1, 0, nScrollRows);
  }
  return true;
}

void BuildFrame::onDraw()
{
  Frame::onDraw();

  if (scroll != 0) {
    shape.colour(1.0f, 1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, style.images.scrollUp);
    shape.fill(x + 112, y + height - HEADER_SIZE - 40, 16, 16);
    glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);
  }
  if (scroll != nScrollRows) {
    shape.colour(1.0f, 1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, style.images.scrollDown);
    shape.fill(x + 112, y + 4, 16, 16);
    glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);
  }

  if (mode == BUILDINGS) {
    for (int i = 0; i < rows * 3; ++i) {
      int index = scroll * 3 + i;

      if (index < techGraph.allowedBuildings.size()) {
        models[i]->setModel(techGraph.allowedBuildings[index]);
        models[i]->show(true);
        models[i]->id = index;
      }
      else {
        models[i]->setModel(nullptr);
        models[i]->show(false);
        models[i]->id = -1;
      }
    }

    nScrollRows = max(0, (techGraph.allowedBuildings.size() + 2) / 3 - rows);
  }
  else {
    const List<const ObjectClass*> allowed = mode == UNITS ? techGraph.allowedUnits :
                                             mode == ITEMS ? techGraph.allowedItems :
                                                             techGraph.allowedObjects;

    for (int i = 0; i < rows * 3; ++i) {
      int index = scroll * 3 + i;

      if (index < allowed.size()) {
        models[i]->setModel(allowed[index]->imagoModel);
        models[i]->show(true);
        models[i]->id = index;
      }
      else {
        models[i]->setModel(-1);
        models[i]->show(false);
        models[i]->id = -1;
      }
    }

    nScrollRows = max(0, (allowed.size() + 2) / 3 - rows);
  }

  if (!isOverModel && wasOverModel) {
    wasOverModel = false;

    switch (mode) {
      case BUILDINGS: {
        title.setText("%s", OZ_GETTEXT("Buildings"));
        break;
      }
      case UNITS: {
        title.setText("%s", OZ_GETTEXT("Units"));
        break;
      }
      case ITEMS: {
        title.setText("%s", OZ_GETTEXT("Items"));
        break;
      }
      case OBJECTS: {
        title.setText("%s", OZ_GETTEXT("Miscellaneous"));
      }
    }
  }

  scroll      = clamp(scroll, 0, nScrollRows);
  isOverModel = false;
}

BuildFrame::BuildFrame() :
  Frame(240, 54, OZ_GETTEXT("Buildings")), mode(BUILDINGS), models(nullptr),
  overlayBSP(nullptr), overlayClass(nullptr), overlayHeading(NORTH),
  rows(0), nScrollRows(0), scroll(0), isOverModel(false), wasOverModel(false)
{}

BuildFrame::~BuildFrame()
{
  delete[] models;
}

}
}
}
