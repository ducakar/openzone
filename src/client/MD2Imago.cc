/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/MD2Imago.cc
 */

#include "stable.hh"

#include "client/MD2Imago.hh"

#include "client/Colours.hh"
#include "client/Context.hh"
#include "client/Camera.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

const float MD2Imago::TURN_SMOOTHING_COEF = 0.25f;

Pool<MD2Imago, 256> MD2Imago::pool;

MD2Imago::~MD2Imago()
{
  context.releaseMD2( clazz->imagoModel );
}

MD2::Anim MD2Imago::extractAnim() const
{
  const Bot*    bot    = static_cast<const Bot*>( obj );
  const Weapon* weapon = bot->weapon == -1 ?
                         null : static_cast<const Weapon*>( orbis.objects[bot->weapon] );

  if( bot->state & Bot::DEAD_BIT ) {
    if( anim.type == MD2::ANIM_DEATH_FALLBACK || anim.type == MD2::ANIM_DEATH_FALLBACKSLOW ||
        anim.type == MD2::ANIM_DEATH_FALLFORWARD )
    {
      return anim.type;
    }
    else {
      return MD2::Anim( MD2::ANIM_DEATH_FALLBACK + Math::rand( 3 ) );
    }
  }
  else if( bot->cell == null ) {
    return MD2::ANIM_CROUCH_STAND;
  }
  else if( ( bot->actions & Bot::ACTION_JUMP ) &&
           !( bot->state & ( Bot::GROUNDED_BIT | Bot::CLIMBING_BIT ) ) )
  {
    return MD2::ANIM_JUMP;
  }
  else if( bot->state & Bot::MOVING_BIT ) {
    return bot->state & Bot::CROUCHING_BIT ? MD2::ANIM_CROUCH_WALK : MD2::ANIM_RUN;
  }
  else if( bot->cargo == -1 ) {
    if( weapon != null && weapon->shotTime != 0.0f ) {
      return bot->state & Bot::CROUCHING_BIT ? MD2::ANIM_CROUCH_ATTACK : MD2::ANIM_ATTACK;
    }
    else if( bot->state & Bot::CROUCHING_BIT ) {
      return MD2::ANIM_CROUCH_STAND;
    }
    else if( bot->state & Bot::GESTURE0_BIT ) {
      return MD2::ANIM_POINT;
    }
    else if( bot->state & Bot::GESTURE1_BIT ) {
      return MD2::ANIM_FALLBACK;
    }
    else if( bot->state & Bot::GESTURE2_BIT ) {
      return MD2::ANIM_SALUTE;
    }
    else if( bot->state & Bot::GESTURE3_BIT ) {
      return MD2::ANIM_WAVE;
    }
    else if( bot->state & Bot::GESTURE4_BIT ) {
      return MD2::ANIM_FLIP;
    }
    else if( anim.type == MD2::ANIM_POINT || anim.type == MD2::ANIM_FALLBACK ||
             anim.type == MD2::ANIM_SALUTE || anim.type == MD2::ANIM_WAVE ||
             anim.type == MD2::ANIM_FLIP )
    {
      return anim.type;
    }
  }
  return bot->state & Bot::CROUCHING_BIT ? MD2::ANIM_CROUCH_STAND : MD2::ANIM_STAND;
}

Imago* MD2Imago::create( const Object* obj )
{
  hard_assert( obj->flags & Object::BOT_BIT );

  const Bot* bot   = static_cast<const Bot*>( obj );
  MD2Imago*  imago = new MD2Imago( obj );

  imago->flags = Imago::MD2MODEL_BIT;
  imago->md2   = context.requestMD2( obj->clazz->imagoModel );
  imago->h     = bot->h;

  imago->anim.type = MD2::ANIM_STAND;
  MD2::setAnim( &imago->anim, imago->extractAnim() );
  imago->anim.currFrame = imago->anim.lastFrame;
  imago->anim.nextFrame = imago->anim.lastFrame;

  return imago;
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
    MD2::Anim desiredAnim = extractAnim();

    if( desiredAnim != anim.type ) {
      MD2::setAnim( &anim, desiredAnim );
    }

    md2->advance( &anim, timer.frameTime );

    // keep animation in sync with weapon shotInterval
    if( anim.type == MD2::ANIM_ATTACK && bot->weapon != -1 ) {
      const Weapon* weapon = static_cast<const Weapon*>( orbis.objects[bot->weapon] );

      if( weapon != null &&
          ( weapon->shotTime == 0.0f ||
            weapon->shotTime == static_cast<const WeaponClass*>( weapon->clazz )->shotInterval ) )
      {
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
          float diffH = Math::fmod( bot->h - h + 1.5f*Math::TAU, Math::TAU ) - 0.5f*Math::TAU;
          h = Math::fmod( h + TURN_SMOOTHING_COEF * diffH + Math::TAU, Math::TAU );

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
