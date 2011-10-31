/*
 *  BasicAudio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/BasicAudio.hpp"

#include "matrix/Orbis.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

Pool<BasicAudio, 1024> BasicAudio::pool;

BasicAudio::BasicAudio( const Object* obj ) : Audio( obj )
{
  oldFlags[0] = 0;
  oldFlags[1] = 0;
}

Audio* BasicAudio::create( const Object* obj )
{
  return new BasicAudio( obj );
}

void BasicAudio::play( const Audio* parent )
{
  const Dynamic* dyn = static_cast<const Dynamic*>( obj );
  const int ( &samples )[ObjectClass::AUDIO_SAMPLES] = obj->clazz->audioSamples;

  // prevent friction sound restarting when it suspends for a tick or two
  int objFlags = oldFlags[0] | oldFlags[1] | obj->flags;
  oldFlags[timer.ticks % 2] = obj->flags;

  // friction
  if( parent == null &&
      ( objFlags & ( Object::DYNAMIC_BIT | Object::FRICTING_BIT | Object::ON_SLICK_BIT ) ) ==
      ( Object::DYNAMIC_BIT | Object::FRICTING_BIT ) && samples[Object::EVENT_FRICTING] != -1 )
  {
    float dvx = dyn->velocity.x;
    float dvy = dyn->velocity.y;

    if( dyn->lower != -1 ) {
      const Dynamic* sDyn = static_cast<const Dynamic*>( orbis.objects[dyn->lower] );

      if( sDyn != null ) {
        dvx -= sDyn->velocity.x;
        dvy -= sDyn->velocity.y;
      }
    }

    playContSound( samples[Object::EVENT_FRICTING], Math::sqrt( dvx*dvx + dvy*dvy ), obj, obj );
  }

  // events
  foreach( event, obj->events.citer() ) {
    hard_assert( event->id < ObjectClass::AUDIO_SAMPLES );

    if( event->id >= 0 && samples[event->id] != -1 ) {
      hard_assert( 0.0f <= event->intensity );

      playSound( samples[event->id], event->intensity, obj, parent == null ? obj : parent->obj );
    }
  }

  // inventory items' events
  foreach( item, obj->items.citer() ) {
    const Object* obj = orbis.objects[*item];

    if( obj != null && ( obj->flags & Object::AUDIO_BIT ) ) {
      context.playAudio( obj, parent == null ? this : parent );
    }
  }
}

}
}
