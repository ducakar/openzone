/*
 *  HealthItem.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "HealthItem.h"

#include "HealthItemClass.h"
#include "Bot.h"
#include "Synapse.h"

namespace oz
{

  void HealthItem::onUse( Bot *bot )
  {
    HealthItemClass *clazz = static_cast<HealthItemClass*>( type );
    bot->life = min( bot->type->life, bot->life + clazz->health );
    synapse.remove( this );
  }

}
