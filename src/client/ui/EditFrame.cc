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

#include <client/ui/EditFrame.hh>

#include <matrix/Synapse.hh>
#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/Camera.hh>
#include <client/Input.hh>
#include <client/ui/Mouse.hh>
#include <client/ui/StrategicArea.hh>
#include <client/ui/UI.hh>

namespace oz::client::ui
{

void EditFrame::itemCallback(ModelField* sender, bool isClicked)
{
  EditFrame*    editFrame = static_cast<EditFrame*>(sender->parent);
  const Object* container = editFrame->owner;
  Object*       item      = nullptr;
  int           id        = editFrame->scrollOwner * COLS + sender->id;

  if (uint(id) < uint(container->items.size())) {
    item = orbis.obj(container->items[id]);
  }
  if (item == nullptr) {
    return;
  }

  OZ_ASSERT(editFrame->taggedItemIndex == -1);

  editFrame->taggedItemIndex = item->index;

  if (isClicked && input.leftReleased) {
    synapse.remove(item);
  }
}

void EditFrame::updateReferences()
{
  if (ui.strategicArea->taggedObjs.size() == 1) {
    owner = orbis.obj(ui.strategicArea->taggedObjs[0]);
  }
  else {
    owner = nullptr;
  }
}

void EditFrame::handleScroll(const Object* container, int* scroll)
{
  if (input.wheelDown) {
    int nScrollRows = max(0, container->clazz->nItems - 1) / COLS;
    *scroll = clamp(*scroll + 1,  0, nScrollRows);
  }
  if (input.wheelUp) {
    int nScrollRows = max(0, container->clazz->nItems - 1) / COLS;
    *scroll = clamp(*scroll - 1,  0, nScrollRows);
  }
}

void EditFrame::drawComponent(int height, const Object* container, const Dynamic* taggedItem,
                              int scroll)
{
  const ObjectClass* containerClazz = container->clazz;

  int nScrollRows = max(0, containerClazz->nItems - 1) / COLS;

  if (scroll != 0) {
    shape.colour(1.0f, 1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, style.images.scrollUp);
    shape.fill(x + 16, y + height + SLOT_SIZE, 16, 16);
    glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);
  }
  if (scroll != nScrollRows) {
    shape.colour(1.0f, 1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, style.images.scrollDown);
    shape.fill(x + 16, y + height - 16, 16, 16);
    glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);
  }

  if (taggedItem == nullptr || taggedItem->parent != container->index) {
    return;
  }

  const ObjectClass* taggedClazz = taggedItem->clazz;

  float life   = taggedItem->life / taggedClazz->life;
  float status = taggedItem->status();

  lifeBar.draw(this, x + width - 52, y + height + SLOT_SIZE + 8, 50, 12, life);
  if (status >= 0.0f) {
    statusBar.draw(this, x + width - 52, y + height + SLOT_SIZE + 1, 50, 8, status);
  }

  itemDesc.setPosition(-ICON_SIZE - 8, height - FOOTER_SIZE / 2);
  itemDesc.setText("#%d %s", taggedItem->index, taggedClazz->title.c());
  itemDesc.draw(this);
}

void EditFrame::onVisibilityChange(bool)
{
  scrollOwner = 0;
}

void EditFrame::onUpdate()
{
  updateReferences();
  taggedItemIndex = -1;

  show(mouse.isVisible && owner != nullptr);
}

bool EditFrame::onMouseEvent()
{
  Frame::onMouseEvent();

  if (input.keys[Input::KEY_UI_ALT]) {
    return true;
  }

  handleScroll(owner, &scrollOwner);
  return true;
}

void EditFrame::onDraw()
{
  updateReferences();

  if (owner == nullptr) {
    return;
  }

  const Object*  container  = owner;
  const Dynamic* taggedItem = orbis.obj<const Dynamic>(taggedItemIndex);

  title.setText("#%d %s", container->index, container->title().c());

  for (int i = 0; i < COLS; ++i) {
    int id = scrollOwner * COLS + i;

    if (id < owner->items.size()) {
      const Object* item = orbis.obj(owner->items[id]);

      ownerModels[i]->show(true);
      ownerModels[i]->setModel(item == nullptr ? -1 : item->clazz->imagoModel);
    }
    else {
      ownerModels[i]->show(id < owner->items.capacity());
      ownerModels[i]->setModel(-1);
    }
  }

  Frame::onDraw();

  drawComponent(FOOTER_SIZE, owner, taggedItem, scrollOwner);
}

EditFrame::EditFrame()
  : Frame(COLS*SLOT_SIZE, SINGLE_HEIGHT, ""),
    owner(nullptr),
    lifeBar(&style.taggedLife), statusBar(&style.taggedStatus),
    itemDesc(-ICON_SIZE - 12, FOOTER_SIZE / 2, 0, ALIGN_RIGHT | ALIGN_VCENTRE, &style.sansFont, ""),
    taggedItemIndex(-1), scrollOwner(0)
{
  flags |= UPDATE_BIT;

  for (int i = 0; i < COLS; ++i) {
    ownerModels[i] = new ModelField(itemCallback, SLOT_SIZE);
    ownerModels[i]->setClickMask(-1);
    ownerModels[i]->id = i;

    add(ownerModels[i], i*SLOT_SIZE, FOOTER_SIZE);
  }
}

}
