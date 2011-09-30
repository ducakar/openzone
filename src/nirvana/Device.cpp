/*
 *  Device.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "nirvana/Device.hpp"

namespace oz
{
namespace nirvana
{

  Device::~Device()
  {}

  const char* Device::getMemo() const
  {
    return null;
  }

  void Device::onUse( const Bot* )
  {}

  void Device::onUpdate()
  {}


}
}
