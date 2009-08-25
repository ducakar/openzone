/*
 *  Mind.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Mind.h"

namespace oz
{

  Mind::~Mind()
  {}

  void Mind::readFull( InputStream *istream )
  {
    botIndex = istream->readInt();
  }

  void Mind::writeFull( OutputStream *ostream )
  {
    ostream->writeInt( botIndex );
  }

}
