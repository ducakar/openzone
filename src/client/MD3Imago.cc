/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/MD3Imago.cc
 */

#include <stable.hh>
#include <client/MD3Imago.hh>

#include <ozEngine/OpenGL.hh>
#include <client/Context.hh>
#include <client/Camera.hh>

namespace oz
{
namespace client
{

const float MD3Imago::TURN_SMOOTHING_COEF = 0.25f;

Pool<MD3Imago, 256> MD3Imago::pool;

Imago* MD3Imago::create( const Object* obj )
{
  hard_assert( obj->flags & Object::BOT_BIT );

  const Bot* bot   = static_cast<const Bot*>( obj );
  MD3Imago*  imago = new MD3Imago( obj );

  imago->flags = Imago::MD3MODEL_BIT;
  imago->md3   = context.requestMD3( obj->clazz->imagoModel );
  imago->h     = bot->h;

//     imago->setAnim( bot->anim );
//     imago->anim.nextFrame = imago->anim.endFrame;
//     imago->anim.currFrame = imago->anim.endFrame;

  return imago;
}

MD3Imago::~MD3Imago()
{
  context.releaseMD3( clazz->imagoModel );
}

//   void MD3Imago::setAnim( Anim::Type type_ )
//   {
//     int type = int( type_ );
//
//     anim.type       = type_;
//     anim.repeat     = MD3::ANIM_LIST[type].repeat;
//
//     anim.startFrame = MD3::ANIM_LIST[type].firstFrame;
//     anim.endFrame   = MD3::ANIM_LIST[type].lastFrame;
//     anim.nextFrame  = anim.startFrame == anim.endFrame ? anim.endFrame : anim.startFrame + 1;
//
//     anim.currTime   = 0.0f;
//
//     if( type_ == Anim::ATTACK ) {
//       const Bot*    bot    = static_cast<const Bot*>( obj );
//       const Weapon* weapon = static_cast<const Weapon*>( orbis.objects[bot->weapon] );
//
//       if( weapon != nullptr ) {
//         const WeaponClass* clazz = static_cast<const WeaponClass*>( weapon->clazz );
//
//         anim.fps       = MD2::ANIM_LIST[type].fps * 0.5f / clazz->shotInterval;
//         anim.frameTime = 1.0f / anim.fps;
//       }
//     }
//     else {
//       anim.fps       = MD2::ANIM_LIST[type].fps;
//       anim.frameTime = 1.0f / anim.fps;
//     }
//   }

void MD3Imago::draw( const Imago* parent )
{
  if( !md3->isLoaded ) {
    return;
  }

  const Bot* bot = static_cast<const Bot*>( obj );
  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

  tf.model = Mat44::translation( obj->p - Point::ORIGIN );

//     if( bot->anim != anim.type ) {
//       setAnim( bot->anim );
//     }

  if( parent == nullptr ) {
    if( !camera.isExternal ) {
      h = bot->h;
    }
    else if( bot->h - h > Math::TAU / 2.0f ) {
      h = bot->h + TURN_SMOOTHING_COEF * ( h + Math::TAU - bot->h );
    }
    else if( h - bot->h > Math::TAU / 2.0f ) {
      h = bot->h + Math::TAU + TURN_SMOOTHING_COEF * ( h - bot->h - Math::TAU );
    }
    else {
      h = bot->h + TURN_SMOOTHING_COEF * ( h - bot->h );
    }

    tf.model.rotateZ( h );
  }

  if( bot->state & Bot::DEAD_BIT ) {
    tf.colour.w.w = min( bot->life * 8.0f / clazz->life, 1.0f );

    tf.model.translate( Vec3( 0.0f, 0.0f, clazz->dim.z - clazz->corpseDim.z ) );

//       md3->advance( &anim, timer.frameTime );
//       md3->draw( &anim );
    md3->drawFrame( 0 );

    tf.colour.w.w = 1.0f;
  }
  else if( bot->index != camera.bot || camera.isExternal ) {
    if( bot->state & Bot::CROUCHING_BIT ) {
      tf.model.translate( Vec3( 0.0f, 0.0f, clazz->dim.z - clazz->crouchDim.z ) );
    }

//       md3->advance( &anim, timer.frameTime );
//       md3->draw( &anim );
    md3->drawFrame( 0 );

    if( parent == nullptr && bot->weapon >= 0 && orbis.objects[bot->weapon] != nullptr ) {
      context.drawImago( orbis.objects[bot->weapon], this );
    }
  }
  else if( parent == nullptr && bot->weapon >= 0 && orbis.objects[bot->weapon] != nullptr ) {
    tf.model.translate( Vec3( 0.0f, 0.0f,  bot->camZ ) );
    tf.model.rotateX( bot->v - Math::TAU / 4.0f );
    tf.model.translate( Vec3( 0.0f, 0.0f, -bot->camZ ) );

//       md2->advance( &anim, timer.frameTime );
    context.drawImago( orbis.objects[bot->weapon], this );
  }
}

}
}
