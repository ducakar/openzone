/*
 *  Model.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Model.h"

namespace oz
{
namespace client
{

  Model::~Model()
  {}

  void Model::drawMounted()
  {
    draw();
  }

}
}
