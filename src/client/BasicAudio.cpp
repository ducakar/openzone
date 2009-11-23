/*
 *  BasicAudio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "BasicAudio.h"

#include "matrix/Physics.h"
#include "Camera.h"
#include "Sound.h"

namespace oz
{
namespace client
{

  const float BasicAudio::MOMENTUM_INTENSITY_FACTOR = -1.0f / 8.0f;

  Audio *BasicAudio::create( const Object *obj )
  {
    assert( ( obj->flags & Object::DYNAMIC_BIT ) || ( obj->flags & Object::BOT_BIT ) );

    return new BasicAudio( obj );
  }

  void BasicAudio::update()
  {
    const DynObject *obj = static_cast<const DynObject*>( this->obj );
    int ( &samples )[ObjectClass::AUDIO_SAMPLES] = obj->type->audioSamples;

    // friction
    if( ( obj->flags & Object::FRICTING_BIT ) && ( ~obj->flags & Object::ON_SLICK_BIT ) &&
        ( obj->waterDepth == 0.0f ) && samples[SND_FRICTING] != -1 )
    {
      float dv = Math::sqrt( obj->velocity.x*obj->velocity.x + obj->velocity.y*obj->velocity.y );
      playContSound( samples[SND_FRICTING], dv, reinterpret_cast<uint>( &*obj ) );
    }

    // events
    foreach( event, obj->events.iterator() ) {
      assert( event->id < ObjectClass::AUDIO_SAMPLES );

      if( event->id >= 0 && samples[event->id] != -1 ) {
        // if intensity field holds intensity of event on interval [0,1]
        if( event->intensity >= 0.0f ) {
          playSound( samples[event->id], event->intensity );
        }
        // if intensity field holds a momentum
        else {
          playSound( samples[event->id], event->intensity * MOMENTUM_INTENSITY_FACTOR );
        }
      }
    }
  }

}
}
