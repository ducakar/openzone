/*
 *  Exit.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  class Exit
  {
    public:

      void add( void ( *callback )() );
      void commit( const char *message );

  };

  extern Exit exit;

}
