/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

void Font::close()
{
  if (handle != nullptr) {
    TTF_Font* font = static_cast<TTF_Font*>(handle);

    TTF_CloseFont(font);
    handle = nullptr;
  }

  fileBuffer.free();
}

Font::Font(const File& file, int height)
  : fontHeight(height), fileBuffer(0)
{
  if (!file.read(&fileBuffer)) {
    OZ_ERROR("oz::Font: Failed to read font file `%s'", file.c());
  }

  SDL_RWops* rwOps = SDL_RWFromConstMem(fileBuffer.begin(), fileBuffer.capacity());
  TTF_Font*  font  = TTF_OpenFontRW(rwOps, true, height);

  if (font == nullptr) {
    OZ_ERROR("oz::Font: %s", TTF_GetError());
  }

  TTF_SetFontHinting(font, TTF_HINTING_MONO);

  handle = font;
}

Font::~Font()
{
  close();
}

Font::Font(Font&& other) noexcept
  : handle(other.handle), fontHeight(other.fontHeight),
    fileBuffer(static_cast<Stream&&>(other.fileBuffer))
{
  OZ_MOVE_CTOR_BODY(Font);
}

Font& Font::operator=(Font&& other) noexcept
{
  OZ_MOVE_OP_BODY(Font);
}

void Font::sizeOf(const char* s, int* width, int* height) const
{
  TTF_Font* font = static_cast<TTF_Font*>(handle);

  TTF_SizeUTF8(font, s, width, height);
}

void Font::upload(const char* s, int* width, int* height) const
{
  TTF_Font*    font    = static_cast<TTF_Font*>(handle);
  SDL_Surface* surface = width != nullptr && *width > 0
                         ? TTF_RenderUTF8_Blended_Wrapped(font, s, WHITE_COLOUR, *width)
                         : TTF_RenderUTF8_Blended(font, s, WHITE_COLOUR);

  if (surface == nullptr) {
    OZ_ERROR("oz::Font: %s", TTF_GetError());
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

void Font::init()
{
  // Set good old hinting. The modern hinter is optimised for subpixel rendering (a.k.a. ClearType)
  // and ignores horizontal hints making old-school TTF fonts look blurry when not using subpixel
  // rendering. This doesn't affect the modern fonts as they usually don't have any horizontal hints
  // at all since they are made with subpixel rendering in mind.
  setenv("FREETYPE_PROPERTIES", "truetype:interpreter-version=35", true);

  if (TTF_Init() < 0) {
    OZ_ERROR("oz::Font: %s", TTF_GetError());
  }
}

void Font::destroy()
{
  TTF_Quit();
}

}
