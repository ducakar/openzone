/*
 *  Stage.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Stage.hpp"

namespace oz
{
namespace client
{

  Stage* Stage::nextStage = null;

  Stage::~Stage()
  {}

}
}
