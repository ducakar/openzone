/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/MD2Imago.cpp
 */

#include "stable.hpp"

#include "client/MD2Imago.hpp"

#include "client/Colours.hpp"
#include "client/Context.hpp"
#include "client/Camera.hpp"
#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

const float MD2Imago::TURN_SMOOTHING_COEF = 0.75f;

Pool<MD2Imago, 256> MD2Imago::pool;

Imago* MD2Imago::create( const Object* obj )
{
  hard_assert( obj->flags & Object::BOT_BIT );

  const Bot* bot   = static_cast<const Bot*>( obj );
  MD2Imago*  imago = new MD2Imago( obj );

  imago->flags = Imago::MD2MODEL_BIT;
  imago->md2   = context.requestMD2( obj->clazz->imagoModel );
  imago->h     = bot->h;

  imago->setAnim( bot->anim );
  imago->anim.currFrame = imago->anim.lastFrame;
  imago->anim.nextFrame = imago->anim.lastFrame;

  return imago;
}

MD2Imago::~MD2Imago()
{
  context.releaseMD2( clazz->imagoModel );
}

void MD2Imago::setAnim( Bot::Anim botAnim )
{
  const Bot* bot = static_cast<const Bot*>( obj );

  switch( botAnim ) {
    case Bot::ANIM_STAND: {
      anim.type = bot->state & Bot::CROUCHING_BIT ? MD2::ANIM_CROUCH_STAND : MD2::ANIM_STAND;
      break;
    }
    case Bot::ANIM_RUN: {
      anim.type = bot->state & Bot::CROUCHING_BIT ? MD2::ANIM_CROUCH_WALK : MD2::ANIM_RUN;
      break;
    }
    case Bot::ANIM_JUMP: {
      anim.type = MD2::ANIM_JUMP;
      break;
    }
    case Bot::ANIM_ATTACK: {
      anim.type = bot->state & Bot::CROUCHING_BIT ? MD2::ANIM_CROUCH_ATTACK : MD2::ANIM_ATTACK;
      break;
    }
    case Bot::ANIM_DEATH: {
      anim.type = MD2::Anim( MD2::ANIM_DEATH_FALLBACK + Math::rand( 3 ) );
      break;
    }
    case Bot::ANIM_GESTURE0: {
      anim.type = MD2::ANIM_POINT;
      break;
    }
    case Bot::ANIM_GESTURE1: {
      anim.type = MD2::ANIM_FALLBACK;
      break;
    }
    case Bot::ANIM_GESTURE2: {
      anim.type = MD2::ANIM_SALUTE;
      break;
    }
    case Bot::ANIM_GESTURE3: {
      anim.type = MD2::ANIM_WAVE;
      break;
    }
    case Bot::ANIM_GESTURE4: {
      anim.type = MD2::ANIM_FLIP;
      break;
    }
  }

  anim.botAnim    = botAnim;
  anim.repeat     = MD2::ANIM_LIST[anim.type].repeat;
  anim.firstFrame = MD2::ANIM_LIST[anim.type].firstFrame;
  anim.lastFrame  = MD2::ANIM_LIST[anim.type].lastFrame;
  anim.nextFrame  = anim.firstFrame == anim.lastFrame ? anim.firstFrame : anim.firstFrame + 1;
  anim.fps        = MD2::ANIM_LIST[anim.type].fps;
  anim.frameTime  = 1.0f / anim.fps;
  anim.currTime   = 0.0f;
}

void MD2Imago::draw( const Imago* parent, int mask )
{
  flags |= UPDATED_BIT;

  if( !md2->isLoaded ) {
    return;
  }

  const Bot*      bot   = static_cast<const Bot*>( obj );
  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

  if( mask & Mesh::SOLID_BIT ) {
    if( bot->anim != anim.botAnim ) {
      setAnim( bot->anim );
    }

    md2->advance( &anim, timer.frameTime );

    // a hack to keep animation in sync with weapon shotInterval
    if( bot->weapon != -1 && orbis.objects[bot->weapon] != null ) {
      const Weapon* weapon = static_cast<const Weapon*>( orbis.objects[bot->weapon] );

      if( !weapon->events.isEmpty() ) {
        anim.nextFrame = anim.firstFrame;
      }
    }

    if( !( bot->state & Bot::DEAD_BIT ) ) {
      if( bot->index == camera.bot && !camera.isExternal ) {
        h = bot->h;

        if( parent == null && bot->weapon != -1 && orbis.objects[bot->weapon] != null ) {
          tf.model = Mat44::translation( obj->p - Point3::ORIGIN );
          tf.model.rotateZ( bot->h );

          tf.model.translate( Vec3( 0.0f, 0.0f, +bot->camZ ) );
          tf.model.rotateX( bot->v - Math::TAU / 4.0f );
          tf.model.translate( Vec3( 0.0f, 0.0f, -bot->camZ ) );

          context.drawImago( orbis.objects[bot->weapon], this, Mesh::SOLID_BIT );
        }
      }
      else {
        if( shader.mode == Shader::SCENE && parent == null ) {
          if( bot->h - h > Math::TAU / 2.0f ) {
            h = Math::mix( bot->h, h + Math::TAU, TURN_SMOOTHING_COEF );
          }
          else if( bot->h - h < -Math::TAU / 2.0f ) {
            h = Math::mix( bot->h + Math::TAU, h, TURN_SMOOTHING_COEF );
          }
          else {
            h = bot->h + TURN_SMOOTHING_COEF * ( h - bot->h );
          }
          h = Math::fmod( h + Math::TAU, Math::TAU );

          tf.model = Mat44::translation( obj->p - Point3::ORIGIN );
          tf.model.rotateZ( h );
        }

        if( bot->state & Bot::CROUCHING_BIT ) {
          tf.model.translate( Vec3( 0.0f, 0.0f, clazz->dim.z - clazz->crouchDim.z ) );
        }

        md2->draw( &anim );

        if( parent == null && bot->weapon!= -1 && orbis.objects[bot->weapon] != null ) {
          context.drawImago( orbis.objects[bot->weapon], this, Mesh::SOLID_BIT );
        }
      }
    }
  }
  else if( bot->state & Bot::DEAD_BIT ) {
    shader.colour.w = min( bot->life * 8.0f / clazz->life, 1.0f );

    if( shader.mode == Shader::SCENE && parent == null ) {
      tf.model = Mat44::translation( obj->p - Point3::ORIGIN );
      tf.model.rotateZ( h );
    }
    tf.model.translate( Vec3( 0.0f, 0.0f, clazz->dim.z - clazz->corpseDim.z ) );

    md2->draw( &anim );

    shader.colour.w = 1.0f;
  }
}

}
}
