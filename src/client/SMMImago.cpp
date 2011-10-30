/*
 *  OBJImago.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/SMMImago.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

Pool<SMMImago, 1024> SMMImago::pool;

Imago* SMMImago::create( const Object* obj )
{
  SMMImago* imago = new SMMImago();

  imago->obj   = obj;
  imago->clazz = obj->clazz;
  imago->smm   = context.requestSMM( obj->clazz->imagoModel );

  return imago;
}

SMMImago::~SMMImago()
{
  context.releaseSMM( clazz->imagoModel );
}

void SMMImago::draw( const Imago*, int mask )
{
  if( !smm->isLoaded ) {
    return;
  }

  tf.model.rotateZ( float( obj->flags & Object::HEADING_MASK ) * Math::TAU / 4.0f );

  smm->draw( mask );
}

}
}
