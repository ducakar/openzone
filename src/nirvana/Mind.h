/*
 *  Mind.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "nirvana.h"

namespace oz
{
namespace nirvana
{

  struct Mind
  {
    friend class DList<Mind>;

    public:

      static const int FORCE_UPDATE_BIT = 0x00000001;

      typedef Mind* ( *CreateFunc )( int bot );
      typedef Mind* ( *ReadFunc )( InputStream* istream );

    private:

      Mind* prev[1];
      Mind* next[1];

    public:

      int botIndex;
      int flags;

      static Mind* create( int botIndex );
      static Mind* read( InputStream* istream );

      Mind( int botIndex_ ) : botIndex( botIndex_ ), flags( 0 ) {}
      virtual ~Mind();

      virtual const char* type() const;
      virtual void update();

      virtual void write( OutputStream* ostream ) const;

  };

}
}
