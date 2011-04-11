/*
 *  Arch.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "arch/Arch.hpp"

namespace oz
{
namespace arch
{

  Arch arch;

  void Arch::apply()
  {}

  void Arch::sync()
  {}

  void Arch::update()
  {}

  void Arch::load( InputStream* /*istream*/ )
  {
    log.print( "Loading Arch ..." );
    log.printEnd( " OK" );
  }

  void Arch::unload( OutputStream* /*ostream*/ )
  {
    log.print( "Unloading Arch ..." );
    log.printEnd( " OK" );
  }

  void Arch::init()
  {
    log.println( "Initialising Arch {" );
    log.indent();

    log.unindent();
    log.println( "}" );
  }

  void Arch::free()
  {
    log.println( "Freeing Arch {" );
    log.indent();

    log.unindent();
    log.println( "}" );
  }

}
}
