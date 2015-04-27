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
 * @file tests/noise.cc
 */

#include <ozCore/ozCore.hh>
#include <ozEngine/ozEngine.hh>
#include <ozFactory/ozFactory.hh>

#include <SDL.h>
#include <noise/noise.h>

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

  uint t0 = Time::clock();

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

  Log() << "populate time: " << (Time::clock() - t0) << " ms";

  t0 = Time::clock();

  Buffer b0(image.pixels, width * height * 4);
  Buffer b1 = b0.compress(1);
  Buffer b2 = b1.decompress();
  Buffer b3 = b2.compress(1);

  Log() << "zlib time: " << (Time::clock() - t0) << " ms";

  Log() << "b0.length() = " << b0.length();
  Log() << "b1.length() = " << b1.length();
  Log() << "b2.length() = " << b2.length();
  Log() << "b3.length() = " << b3.length();

  Log() << "b0 == b2: " << (mCompare(b0.begin(), b2.begin(), b0.length()) == 0);
  Log() << "b1 == b3: " << (mCompare(b1.begin(), b3.begin(), b1.length()) == 0);

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
    Time::sleep(20);
  }

  Window::destroy();
  SDL_Quit();
  return 0;
}
