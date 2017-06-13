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

#include <ozCore/ozCore.hh>
#include <ozEngine/ozEngine.hh>
#include <ozFactory/ozFactory.hh>

#include <noise/noise.h>
#include <SDL2/SDL.h>

using namespace oz;
using namespace noise;

struct Colour
{
  double margin;
  double colour[3];
};

static const int width  = 1024;
static const int height = 1024;

int main(int, char**)
{
  System::init();
  SDL_Init(SDL_INIT_VIDEO);
  Window::create("test", 800, 800);

  Duration t0 = Time::clock();

  TerraBuilder::addGradientPoint(Vec4(0.00f, 0.00f, 0.10f, -1.00f));
  TerraBuilder::addGradientPoint(Vec4(0.00f, 0.20f, 0.40f, -0.20f));
  TerraBuilder::addGradientPoint(Vec4(0.20f, 0.60f, 0.60f, -0.00f));
  TerraBuilder::addGradientPoint(Vec4(0.80f, 0.60f, 0.20f, +0.05f));
  TerraBuilder::addGradientPoint(Vec4(0.10f, 0.40f, 0.15f, +0.20f));
  TerraBuilder::addGradientPoint(Vec4(0.05f, 0.30f, 0.10f, +0.50f));
  TerraBuilder::addGradientPoint(Vec4(0.50f, 0.50f, 0.50f, +0.80f));
  TerraBuilder::addGradientPoint(Vec4(0.80f, 0.80f, 0.80f, +0.95f));

  ImageData image = TerraBuilder::generateImage(width, height);

  ImageBuilder::createDDS(&image, 1, "drek.dds");

  Log() << "populate time: " << (Time::clock() - t0).ms() << " ms";

  bool      isAlive = true;
  SDL_Event event;

  uint texId;
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);

  GL::textureDataFromFile("drek.dds");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glEnable(GL_TEXTURE_2D);

  while (isAlive) {
    SDL_PollEvent(&event);
    isAlive &= event.type != SDL_KEYUP;

    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_QUADS);
      glTexCoord2d(0, 1); glVertex2d(-1, -1);
      glTexCoord2d(1, 1); glVertex2d(+1, -1);
      glTexCoord2d(1, 0); glVertex2d(+1, +1);
      glTexCoord2d(0, 0); glVertex2d(-1, +1);
    glEnd();

    Window::swapBuffers();
    Time::sleep(20_ms);
  }

  Window::destroy();
  SDL_Quit();
  return 0;
}
