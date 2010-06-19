/*
 *  Task.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "stable.hpp"

#include "nirvana/Task.hpp"

namespace oz
{
namespace nirvana
{

  void Task::write( OutputStream* ostream ) const
  {
    foreach( child, children.citer() ) {
      ostream->writeString( child->type() );
      child->write( ostream );
    }
    ostream->writeInt( flags );
  }

}
}
