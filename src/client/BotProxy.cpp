/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/BotProxy.cpp
 */

#include "stable.hpp"

#include "client/BotProxy.hpp"

#include "matrix/Collider.hpp"
#include "matrix/BotClass.hpp"
#include "matrix/VehicleClass.hpp"

#include "client/ui/UI.hpp"

#include "client/Camera.hpp"

namespace oz
{
namespace client
{

const float BotProxy::THIRD_PERSON_CLIP_DIST = 0.20f;
const float BotProxy::BOB_SUPPRESSION_COEF   = 0.60f;

void BotProxy::begin()
{
  if( camera.bot == -1 ) {
    return;
  }

  Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

  bobPhi     = 0.0f;
  bobTheta   = 0.0f;
  bobBias    = 0.0f;

  camera.h = bot->h;
  camera.v = bot->v;
  camera.isExternal = isExternal;
  camera.setTagged( null );

  ui::ui.strategicArea->tagged.clear();
  ui::ui.strategicArea->show( false );
  ui::ui.hudArea->show( true );
  ui::mouse.doShow = false;
}

void BotProxy::update()
{
  if( camera.bot == -1 ) {
    return;
  }

  Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

  const char* keys    = ui::keyboard.keys;
  const char* oldKeys = ui::keyboard.oldKeys;

  if( camera.allowReincarnation && keys[SDLK_i] && !oldKeys[SDLK_i] ) {
    bot->actions = 0;
    camera.setBot( null );
    return;
  }

  /*
   * Camera
   */
  if( isFreelook ) {
    camera.h += camera.relH;
    camera.v += camera.relV;
  }
  else {
    float relH = camera.relH;
    float relV = camera.relV;

    // TODO
//       if( bot->parent != -1 && orbis.objects[bot->parent] != null ) {
//         const Vehicle*      vehicle = static_cast<const Vehicle*>( orbis.objects[bot->parent] );
//         const VehicleClass* clazz   = static_cast<const VehicleClass*>( vehicle->clazz );
//
//         relH = clamp( relH, -clazz->turnLimitH, +clazz->turnLimitH );
//         relV = clamp( relV, -clazz->turnLimitV, +clazz->turnLimitV );
//       }

    bot->h += relH;
    bot->v += relV;
  }

  bot->actions = 0;

  /*
   * Movement
   */
  if( keys[SDLK_w] ) {
    bot->actions |= Bot::ACTION_FORWARD;
  }
  if( keys[SDLK_s] ) {
    bot->actions |= Bot::ACTION_BACKWARD;
  }
  if( keys[SDLK_d] ) {
    bot->actions |= Bot::ACTION_RIGHT;
  }
  if( keys[SDLK_a] ) {
    bot->actions |= Bot::ACTION_LEFT;
  }

  /*
   * Actions
   */
  if( keys[SDLK_SPACE] ) {
    bot->actions |= Bot::ACTION_JUMP | Bot::ACTION_VEH_UP;
  }
  if( keys[SDLK_LCTRL] ) {
    bot->actions |= Bot::ACTION_CROUCH | Bot::ACTION_VEH_DOWN;
  }
  if( keys[SDLK_LSHIFT] && !oldKeys[SDLK_LSHIFT] ) {
    bot->state ^= Bot::RUNNING_BIT;
  }
  if( keys[SDLK_x] && oldKeys[SDLK_x] ) {
    bot->actions |= Bot::ACTION_EXIT;
  }
  if( ( keys[SDLK_LALT] || keys[SDLK_RALT] ) && keys[SDLK_x] && !oldKeys[SDLK_x] ) {
    bot->actions |= Bot::ACTION_EJECT;
  }
  if( ( keys[SDLK_LALT] || keys[SDLK_RALT] ) && keys[SDLK_m] && !oldKeys[SDLK_m] ) {
    bot->actions |= Bot::ACTION_SUICIDE;
  }
  if( keys[SDLK_KP_MULTIPLY] && !oldKeys[SDLK_KP_MULTIPLY] ) {
    isFreelook = !isFreelook;

    camera.h = bot->h;
    camera.v = bot->v;
  }
  if( ( keys[SDLK_LALT] || keys[SDLK_RALT] ) && keys[SDLK_p] && !oldKeys[SDLK_p] ) {
    bot->state ^= Bot::STEPABLE_BIT;
  }

  bot->state &= ~( Bot::GESTURE0_BIT | Bot::GESTURE1_BIT | Bot::GESTURE2_BIT |
      Bot::GESTURE3_BIT | Bot::GESTURE4_BIT | Bot::GESTURE_ALL_BIT );

  if( keys[SDLK_f] ) {
    bot->state |= Bot::GESTURE0_BIT;
  }
  if( keys[SDLK_g] ) {
    bot->state |= Bot::GESTURE1_BIT;
  }
  if( keys[SDLK_h] ) {
    bot->state |= Bot::GESTURE2_BIT;
  }
  if( keys[SDLK_j] ) {
    bot->state |= Bot::GESTURE3_BIT;
  }
  if( keys[SDLK_k] ) {
    bot->state |= Bot::GESTURE4_BIT;
  }
  if( keys[SDLK_l] ) {
    bot->state |= Bot::GESTURE_ALL_BIT;
  }

  if( keys[SDLK_KP_ENTER] && !oldKeys[SDLK_KP_ENTER] ) {
    camera.h = bot->h;
    camera.v = bot->v;

    isExternal = !isExternal;
    camera.isExternal = isExternal;

    if( !isExternal ) {
      if( bot->parent != -1 ) {
        camera.align();
        camera.warp( bot->p + camera.up * bot->camZ );
      }
      else {
        camera.warp( bot->p + Vec3( 0.0f, 0.0f, bot->camZ ) );
      }
    }
  }

  if( !ui::mouse.doShow ) {
    if( ui::mouse.buttons & SDL_BUTTON_LMASK ) {
      bot->actions |= Bot::ACTION_ATTACK;
    }
    if( ui::mouse.rightClick ) {
      bot->actions |= Bot::ACTION_USE | Bot::ACTION_VEH_NEXT_WEAPON;

      if( camera.taggedObj != null && ( camera.taggedObj->flags & Object::DEVICE_BIT ) ) {
        ui::mouse.doShow = true;
      }
    }
    if( ui::mouse.wheelDown ) {
      bot->actions |= Bot::ACTION_TAKE;

      if( camera.taggedObj != null && ( camera.taggedObj->flags & Object::BROWSABLE_BIT ) ) {
        ui::mouse.doShow = true;
      }
    }
    if( ui::mouse.wheelUp ) {
      bot->actions |= Bot::ACTION_THROW;
    }
    if( ui::mouse.middleClick ) {
      bot->actions |= Bot::ACTION_GRAB;
    }
  }
}

void BotProxy::prepare()
{
  if( camera.bot == -1 ) {
    camera.setState( camera.defaultState );
    return;
  }

  const Bot* bot = camera.botObj;

  if( !isFreelook ) {
    camera.h = bot->h;
    camera.v = bot->v;
  }

  if( !isExternal ) {
    if( bot->parent != -1 ) { // inside vehicle
      hard_assert( orbis.objects[bot->parent] == null ||
          ( orbis.objects[bot->parent]->flags & Object::VEHICLE_BIT ) );

      camera.w = 0.0f;
      camera.align();
      camera.warp( bot->p + camera.up * bot->camZ );

      bobPhi   = 0.0f;
      bobTheta = 0.0f;
      bobBias  = 0.0f;
    }
    else { // 1st person, not in vehicle
      const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

      if( ( bot->state & ( Bot::MOVING_BIT | Bot::SWIMMING_BIT | Bot::CLIMBING_BIT ) ) ==
          Bot::MOVING_BIT )
      {
        float bobInc =
            ( bot->state & ( Bot::RUNNING_BIT | Bot::CROUCHING_BIT | Bot::GRAB_BIT ) ) ==
              Bot::RUNNING_BIT ? clazz->bobRunInc : clazz->bobWalkInc;

        bobPhi   = Math::mod( bobPhi + bobInc, Math::TAU );
        bobTheta = Math::sin( bobPhi ) * clazz->bobRotation;
        bobBias  = Math::sin( 2.0f * bobPhi ) * clazz->bobAmplitude;
      }
      else if( ( bot->state & ( Bot::MOVING_BIT | Bot::SWIMMING_BIT | Bot::CLIMBING_BIT ) ) ==
          ( Bot::MOVING_BIT | Bot::SWIMMING_BIT ) )
      {
        float bobInc =
            ( bot->state & ( Bot::RUNNING_BIT | Bot::CROUCHING_BIT ) ) == Bot::RUNNING_BIT ?
            clazz->bobSwimRunInc : clazz->bobSwimInc;

        bobPhi   = Math::mod( bobPhi + bobInc, Math::TAU );
        bobTheta = 0.0f;
        bobBias  = Math::sin( 2.0f * bobPhi ) * clazz->bobSwimAmplitude;
      }
      else {
        bobPhi   = 0.0f;
        bobTheta *= BOB_SUPPRESSION_COEF;
        bobBias  *= BOB_SUPPRESSION_COEF;
      }

      Point3 p = bot->p;
      p.z += bot->camZ + bobBias;

      camera.w = bobTheta;
      camera.align();
      camera.warpMoveZ( p );
    }
  }
  else { // external
    camera.w = 0.0f;
    camera.align();

    float dist;
    if( bot->parent != -1 && orbis.objects[bot->parent] ) {
      Vehicle* veh = static_cast<Vehicle*>( orbis.objects[bot->parent] );

      hard_assert( veh->flags & Object::VEHICLE_BIT );

      dist = veh->dim.fastL() * externalDistFactor;
    }
    else {
      dist = bot->dim.fastL() * externalDistFactor;
    }

    Point3 origin = bot->p + Vec3( 0.0f, 0.0f, bot->camZ );
    Vec3   offset = -camera.at * dist;

    collider.translate( origin, offset, bot );

    offset *= collider.hit.ratio;
    offset += camera.at * THIRD_PERSON_CLIP_DIST;

    camera.warpMoveZ( origin + offset );

    bobPhi   = 0.0f;
    bobTheta = 0.0f;
    bobBias  = 0.0f;
  }

  if( bot->parent != -1 ) {
    camera.setTagged( null );
  }
  else if( bot->state & Bot::GRAB_BIT ) {
    camera.setTagged( orbis.objects[camera.botObj->instrument] );
  }
  else if( isFreelook ) {
    // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
    float hvsc[6];

    Math::sincos( bot->h, &hvsc[0], &hvsc[1] );
    Math::sincos( bot->v, &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[2] * hvsc[0];
    hvsc[5] = hvsc[2] * hvsc[1];

    // at vector must be based on bot's orientation, not on camera's
    Vec3 at = Vec3( -hvsc[4], hvsc[5], -hvsc[3] );

    float distance = static_cast<const BotClass*>( camera.botObj->clazz )->reachDist;
    collider.mask = ~0;
    collider.translate( camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ ),
                        at * distance,
                        camera.botObj );
    collider.mask = Object::SOLID_BIT;

    camera.setTagged( collider.hit.obj );
  }
  else {
    float distance = static_cast<const BotClass*>( camera.botObj->clazz )->reachDist;
    collider.mask = ~0;
    collider.translate( camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ ),
                        camera.at * distance,
                        camera.botObj );
    collider.mask = Object::SOLID_BIT;

    camera.setTagged( collider.hit.obj );
  }
}

void BotProxy::reset()
{
  isExternal = config.getSet( "botProxy.isExternal", false );
  isFreelook = false;
}

void BotProxy::read( InputStream* istream )
{
  bobPhi   = 0.0f;
  bobTheta = 0.0f;
  bobBias  = 0.0f;

  isExternal = istream->readBool();
  isFreelook = istream->readBool();
}

void BotProxy::write( OutputStream* ostream ) const
{
  ostream->writeBool( isExternal );
  ostream->writeBool( isFreelook );
}

void BotProxy::init()
{
  externalDistFactor = config.getSet( "botProxy.externalDistFactor", 2.75f );
}

}
}
