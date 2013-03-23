/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

/**
 * @file ozEngine/ALSource.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * OpenAL source wrapper.
 */
class ALSource
{
  private:

    uint sourceId; ///< OpenAL source id, 0 if not created.

  public:

    /**
     * Create an empty instance (no OpenAL source is created).
     */
    explicit ALSource();

    /**
     * Destructor, destroys OpenAL source if created.
     */
    ~ALSource();

    /**
     * Move constructor.
     */
    ALSource( ALSource&& s ) :
      sourceId( s.sourceId )
    {
      s.sourceId = 0;
    }

    /**
     * Move operator.
     */
    ALSource& operator = ( ALSource&& s )
    {
      if( &s == this ) {
        return *this;
      }

      destroy();

      sourceId   = s.sourceId;
      s.sourceId = 0;

      return *this;
    }

    /**
     * Get OpenAL source id.
     */
    uint id() const
    {
      return sourceId;
    }

    /**
     * True iff created.
     */
    bool isCreated() const
    {
      return sourceId != 0;
    }

    /**
     * Create a new uninitialised OpenAL source.
     */
    bool create();

    /**
     * Destroy OpenAL source if created.
     */
    void destroy();

};

}
