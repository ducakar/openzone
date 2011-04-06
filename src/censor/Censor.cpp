/*
 *  Censor.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "censor/Censor.hpp"

namespace oz
{
namespace censor
{

  Censor censor;

  void Censor::apply()
  {}

  void Censor::sync()
  {}

  void Censor::update()
  {}

  void Censor::load( InputStream* /*istream*/ )
  {
    log.print( "Loading Censor ..." );
    log.printEnd( " OK" );
  }

  void Censor::unload( OutputStream* /*ostream*/ )
  {
    log.print( "Unloading Censor ..." );
    log.printEnd( " OK" );
  }

  void Censor::init()
  {
    log.println( "Initialising Censor {" );
    log.indent();

    log.unindent();
    log.println( "}" );
  }

  void Censor::free()
  {
    log.println( "Freeing Censor {" );
    log.indent();

    log.unindent();
    log.println( "}" );
  }

}
}
