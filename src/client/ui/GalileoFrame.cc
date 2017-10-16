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

#include <client/ui/GalileoFrame.hh>

#include <nirvana/QuestList.hh>
#include <client/Shape.hh>
#include <client/Camera.hh>
#include <client/Context.hh>
#include <client/Input.hh>
#include <client/ui/Style.hh>
#include <client/ui/Mouse.hh>

namespace oz::client::ui
{

void GalileoFrame::onRealign()
{
  int maxSize     = camera.height < camera.width ? camera.height - 64 : camera.width - 64;

  normalX         = defaultX < 0
                    ? parent->x + parent->width - normalWidth + defaultX
                    : parent->x + defaultX;
  normalY         = defaultY < 0
                    ? parent->y + parent->height - normalHeight + defaultY
                    : parent->y + defaultY;

  maximisedX      = camera.centreX - maxSize / 2;
  maximisedY      = camera.centreY - maxSize / 2;
  maximisedWidth  = maxSize;
  maximisedHeight = maxSize;

  setMaximised(isMaximised);

  clickX = Math::NaN;
  clickY = Math::NaN;
}

void GalileoFrame::onUpdate()
{
  const Bot* bot = camera.botObj;

  clickX = Math::NaN;
  clickY = Math::NaN;

  if (orbis.terra.id == -1 ||
      (camera.state == Camera::UNIT &&
       (bot == nullptr ||
        (bot->state & Bot::DEAD_BIT) || !bot->hasAttribute(ObjectClass::GALILEO_BIT))))
  {
    show(false);
    setMaximised(false);
  }
  else {
    show(true);
  }
}

bool GalileoFrame::onMouseEvent()
{
  if (input.buttons) {
    clickX = -Orbis::DIM + float(mouse.x - x) / float(width ) * 2.0f * Orbis::DIM;
    clickY = -Orbis::DIM + float(mouse.y - y) / float(height) * 2.0f * Orbis::DIM;
  }
  return true;
}

void GalileoFrame::onDraw()
{
  if (mapTexId == 0) {
    glGenTextures(1, &mapTexId);
    glBindTexture(GL_TEXTURE_2D, mapTexId);
    GL::textureDataFromFile("@terra/" + liber.terrae[orbis.terra.id].name + ".dds",
                            context.textureLod);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  float pX = camera.p.x;
  float pY = camera.p.y;
  float h  = camera.botObj == nullptr ? camera.strategic.h : camera.botObj->h;

  shape.colour(colour);
  glBindTexture(GL_TEXTURE_2D, mapTexId);
  shape.fill(x, y, width, height);
  shape.colour(1.0f, 1.0f, 1.0f, 1.0f);

  float oX      = float(x);
  float oY      = float(y);
  float fWidth  = float(width);
  float fHeight = float(height);

  if (questList.activeQuest != -1) {
    const Quest& quest = questList.quests[questList.activeQuest];

    glBindTexture(GL_TEXTURE_2D, style.images.marker);

    float mapX = oX + (Orbis::DIM + quest.place.x) / (2.0f*Orbis::DIM) * fWidth;
    float mapY = oY + (Orbis::DIM + quest.place.y) / (2.0f*Orbis::DIM) * fHeight;

    tf.model = Mat4::translation(Vec3(mapX, mapY, 0.0f));
    tf.model.scale(Vec3(16.0f, 16.0f, 0.0f));
    tf.model.translate(Vec3(-0.5f, -0.5f, 0.0f));
    tf.apply();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  glBindTexture(GL_TEXTURE_2D, style.images.arrow);

  float mapX = oX + (Orbis::DIM + pX) / (2.0f*Orbis::DIM) * fWidth;
  float mapY = oY + (Orbis::DIM + pY) / (2.0f*Orbis::DIM) * fHeight;

  tf.model = Mat4::translation(Vec3(mapX, mapY, 0.0f));
  tf.model.rotateZ(h);
  tf.model.scale(Vec3(10.0f, 10.0f, 0.0f));
  tf.model.translate(Vec3(-0.5f, -0.5f, 0.0f));
  tf.apply();

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);
}

GalileoFrame::GalileoFrame()
  : Frame(240, 232 - HEADER_SIZE, ""), mapTexId(0), colour(style.colours.galileoNormal),
    clickX(Math::NaN), clickY(Math::NaN), isMaximised(false)
{
  flags |= UPDATE_BIT | PINNED_BIT;

  normalWidth  = width;
  normalHeight = height;
}

GalileoFrame::~GalileoFrame()
{
  if (mapTexId != 0) {
    glDeleteTextures(1, &mapTexId);
  }
}

void GalileoFrame::setMaximised(bool doMaximise)
{
  isMaximised = doMaximise;

  if (doMaximise) {
    x      = maximisedX;
    y      = maximisedY;
    width  = maximisedWidth;
    height = maximisedHeight;
    colour = style.colours.galileoMaximised;

    raise();
  }
  else {
    x      = normalX;
    y      = normalY;
    width  = normalWidth;
    height = normalHeight;
    colour = style.colours.galileoNormal;
  }
}

}
