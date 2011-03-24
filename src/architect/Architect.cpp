/*
 *  Architect.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "architect/Architect.hpp"

namespace oz
{
namespace architect
{

  Architect architect;

  void Architect::apply()
  {}

  void Architect::sync()
  {}

  void Architect::update()
  {}

  void Architect::load( InputStream* /*istream*/ )
  {
    log.print( "Loading Architect ..." );
    log.printEnd( " OK" );
  }

  void Architect::unload( OutputStream* /*ostream*/ )
  {
    log.print( "Unloading Architect ..." );
    log.printEnd( " OK" );
  }

  void Architect::init()
  {
    log.println( "Initialising Architect {" );
    log.indent();

    log.unindent();
    log.println( "}" );
  }

  void Architect::free()
  {
    log.println( "Freeing Architect {" );
    log.indent();

    log.unindent();
    log.println( "}" );
  }

}
}
