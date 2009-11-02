/*
 *  Nirvana.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Mind.h"

namespace oz
{

  class Nirvana
  {
    private:

      struct MindCtor
      {
        Mind::CreateFunc create;
        Mind::ReadFunc   read;

        MindCtor( Mind::CreateFunc create_, Mind::ReadFunc read_ ) :
            create( create_ ), read( read_ )
        {}
      };

      HashString<MindCtor, 31> mindClasses;

      DList<Mind, 0> minds;

      void sync();
      void update();

      static int run( void *data );

      void read( InputStream *istream );
      void write( OutputStream *ostream ) const;

    public:

      SDL_semaphore *semaphore;
      SDL_Thread    *thread;

      volatile bool isAlive;

      void init();
      void free();

      void load( InputStream *istream );
      void unload( OutputStream *ostream );

      void start();
      void stop();

  };

  extern Nirvana nirvana;

}
