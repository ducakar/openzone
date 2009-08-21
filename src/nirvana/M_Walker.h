/*
 *  M_Walker.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Mind.h"

namespace oz
{

  class M_Walker : public Mind
  {
    protected:

      void onUpdate();

    public:

      M_Walker( Bot *bot ) : Mind( bot ) {}

  };

}
