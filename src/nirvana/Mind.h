/*
 *  Mind.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "matrix/Bot.h"

namespace oz
{

  class Mind
  {
    friend class DList<Mind, 0>;

    public:

      typedef Mind *( *CreateFunc )( int botIndex );
      typedef Mind *( *ReadFunc )( InputStream *istream );

    private:

      Mind *prev[1];
      Mind *next[1];

    public:

      int botIndex;

      static Mind *create( int botIndex );
      static Mind *read( InputStream *istream );

      Mind( int botIndex_ ) : botIndex( botIndex_ ) {}
      virtual ~Mind();

      virtual const char *type() const;
      virtual void update();

      virtual void write( OutputStream *ostream ) const;

  };

}
