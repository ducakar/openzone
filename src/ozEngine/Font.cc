/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "Font.hh"

#include "GL.hh"

#include <SDL2/SDL_ttf.h>

namespace oz
{

static const SDL_Color WHITE_COLOUR = {0xff, 0xff, 0xff, 0xff};

Font::Font(const File& file, int height) :
  fontHeight(height), fileBuffer(file.read())
{
  if (fileBuffer.available() == 0) {
    OZ_ERROR("Failed to read font file '%s'", file.c());
  }

  SDL_RWops* rwOps = SDL_RWFromConstMem(fileBuffer.begin(), fileBuffer.capacity());

  handle = TTF_OpenFontRW(rwOps, true, height);
  if (handle == nullptr) {
    OZ_ERROR("%s", TTF_GetError());
  }
}

Font::~Font()
{
  destroy();
}

Font::Font(Font&& f) :
  handle(f.handle), fontHeight(f.fontHeight), fileBuffer(static_cast<Stream&&>(f.fileBuffer))
{
  f.handle     = nullptr;
  f.fontHeight = 0;
}

Font& Font::operator = (Font&& f)
{
  if (&f != this) {
    destroy();

    handle     = f.handle;
    fontHeight = f.fontHeight;
    fileBuffer = static_cast<Stream&&>(f.fileBuffer);

    f.handle     = nullptr;
    f.fontHeight = 0;
  }
  return *this;
}

void Font::sizeOf(const char* s, int* width, int* height) const
{
  TTF_Font* font = static_cast<TTF_Font*>(handle);

  TTF_SizeUTF8(font, s, width, height);
}

void Font::upload(const char* s, int* width, int* height) const
{
  TTF_Font*    font    = static_cast<TTF_Font*>(handle);
  SDL_Surface* surface = width != nullptr && *width > 0 ?
                         TTF_RenderUTF8_Blended_Wrapped(font, s, WHITE_COLOUR, *width) :
                         TTF_RenderUTF8_Blended(font, s, WHITE_COLOUR);

  if (surface == nullptr) {
    OZ_ERROR("Failed to generate texture from text: %s", s);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, surface->pixels);

  if (width != nullptr) {
    *width = surface->w;
  }
  if (height != nullptr) {
    *height = surface->h;
  }

  SDL_FreeSurface(surface);
}

void Font::destroy()
{
  if (handle != nullptr) {
    TTF_Font* font = static_cast<TTF_Font*>(handle);

    TTF_CloseFont(font);
    handle = nullptr;
  }

  fileBuffer.free();
}

}
