/*
 *  HealthItem.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
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
    HealthItemClass *clazz = (HealthItemClass*) type;
    bot->life = min( bot->type->life, bot->life + clazz->health );
    synapse.remove( this );
  }

}
