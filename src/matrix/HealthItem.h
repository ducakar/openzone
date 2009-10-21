/*
 *  HealthItem.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "DynObject.h"

namespace oz
{

  class HealthItem : public DynObject
  {
    protected:

      virtual void onUse( Bot *bot );

  };

}

