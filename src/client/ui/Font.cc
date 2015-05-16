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
 * @file client/ui/Font.cc
 */

#include <client/ui/Font.hh>

#include <SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

static const SDL_Colour SDL_COLOUR_WHITE = { 0xff, 0xff, 0xff, 0xff };

void Font::sizeOf(const char* s, int* width, int* height) const
{
  TTF_Font* font = static_cast<TTF_Font*>(handle);

  TTF_SizeUTF8(font, s, width, height);
}

void Font::upload(const char* s, int* width, int* height) const
{
  TTF_Font*    font = static_cast<TTF_Font*>(handle);
  SDL_Surface* surf = width != nullptr && *width > 0 ?
                      TTF_RenderUTF8_Blended_Wrapped(font, s, SDL_COLOUR_WHITE, *width) :
                      TTF_RenderUTF8_Blended(font, s, SDL_COLOUR_WHITE);

  if (surf == nullptr) {
    OZ_ERROR("Failed to generate texture from text: %s", s);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               surf->pixels);

  if (width != nullptr) {
    *width = surf->w;
  }
  if (height != nullptr) {
    *height = surf->h;
  }

  SDL_FreeSurface(surf);
}

void Font::init(const char* name, int height_)
{
  height = height_;

  File file = String::format("@ui/font/%s.ttf", name);

  buffer = file.read();
  if (buffer.isEmpty()) {
    OZ_ERROR("Failed to read font file '%s'", file.path().c());
  }

  SDL_RWops* rwOps = SDL_RWFromConstMem(buffer.begin(), buffer.length());

  handle = TTF_OpenFontRW(rwOps, true, height);
  if (handle == nullptr) {
    OZ_ERROR("%s", TTF_GetError());
  }
}

void Font::destroy()
{
  if (handle != nullptr) {
    TTF_Font* font = static_cast<TTF_Font*>(handle);

    TTF_CloseFont(font);
    handle = nullptr;
  }

  buffer.resize(0, true);
}

}
}
}
