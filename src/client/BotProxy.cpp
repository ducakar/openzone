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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/BotProxy.cpp
 */

#include "stable.hpp"

#include "client/BotProxy.hpp"

#include "matrix/Collider.hpp"

#include "client/Camera.hpp"
#include "client/ui/UI.hpp"

namespace oz
{
namespace client
{

const float BotProxy::THIRD_PERSON_DIST      = 2.75f;
const float BotProxy::THIRD_PERSON_CLIP_DIST = 0.20f;
const float BotProxy::BOB_SUPPRESSION_COEF   = 0.60f;

BotProxy::BotProxy() : hud( null ), infoFrame( null ), inventory( null ), container( null )
{}

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

  ui::mouse.doShow = false;

  hud       = new ui::HudArea();
  infoFrame = new ui::InfoFrame();
  inventory = new ui::InventoryMenu( null );
  container = new ui::InventoryMenu( inventory );

  ui::ui.root->add( hud );
  ui::ui.root->add( infoFrame );
  ui::ui.root->add( inventory );
  ui::ui.root->add( container );

  infoFrame->show( true );
  inventory->show( false );
  container->show( false );
}

void BotProxy::end()
{
  if( container != null ) {
    ui::ui.root->remove( container );
    container = null;
  }
  if( inventory != null ) {
    ui::ui.root->remove( inventory );
    inventory = null;
  }
  if( infoFrame != null ) {
    ui::ui.root->remove( infoFrame );
    infoFrame = null;
  }
  if( hud != null ) {
    ui::ui.root->remove( hud );
    hud = null;
  }

  ui::mouse.doShow = true;
}

void BotProxy::update()
{
  if( camera.bot == -1 ) {
    return;
  }

  Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

  const char* keys    = ui::keyboard.keys;
  const char* oldKeys = ui::keyboard.oldKeys;

  if( ui::keyboard.keys[SDLK_TAB] && !ui::keyboard.oldKeys[SDLK_TAB] ) {
    ui::mouse.doShow = !ui::mouse.doShow;

    inventory->show( ui::mouse.doShow );
  }

  if( keys[SDLK_KP_ENTER] && !oldKeys[SDLK_KP_ENTER] ) {
    camera.h = bot->h;
    camera.v = bot->v;

    isExternal = !isExternal;
    camera.isExternal = isExternal;

    if( !isExternal ) {
      if( bot->parent != -1 ) {
        camera.warp( bot->p + camera.up * bot->camZ );
      }
      else {
        camera.warp( bot->p + Vec3( 0.0f, 0.0f, bot->camZ ) );
      }
    }
  }

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

//     if( bot->parent != -1 && orbis.objects[bot->parent] != null ) {
//       const Vehicle*      vehicle = static_cast<const Vehicle*>( orbis.objects[bot->parent] );
//       const VehicleClass* clazz   = static_cast<const VehicleClass*>( vehicle->clazz );
//
//       relH = clamp( relH, -clazz->turnLimitH, +clazz->turnLimitH );
//       relV = clamp( relV, -clazz->turnLimitV, +clazz->turnLimitV );
//     }

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
  if( keys[SDLK_x] && !oldKeys[SDLK_x] ) {
    if( keys[SDLK_LALT] || keys[SDLK_RALT] ) {
      bot->actions |= Bot::ACTION_EJECT;
    }
    else {
      bot->actions |= Bot::ACTION_EXIT;
    }
  }
  if( ( keys[SDLK_LALT] || keys[SDLK_RALT] ) && keys[SDLK_m] && !oldKeys[SDLK_m] ) {
    bot->actions |= Bot::ACTION_SUICIDE;
  }
  if( keys[SDLK_KP_MULTIPLY] && !oldKeys[SDLK_KP_MULTIPLY] ) {
    isFreelook = !isFreelook;

    camera.h = bot->h;
    camera.v = bot->v;
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

        inventory->show( true );
        container->show( true );
      }
    }
    if( ui::mouse.wheelUp ) {
      bot->actions |= Bot::ACTION_THROW;
    }
    if( ui::mouse.middleClick ) {
      bot->actions |= Bot::ACTION_GRAB;
    }
    if( keys[SDLK_q] && !oldKeys[SDLK_q] ) {
      bot->actions |= Bot::ACTION_ROTATE;
    }
  }
}

void BotProxy::prepare()
{
  if( camera.bot == -1 ) {
    camera.setState( Camera::STRATEGIC );
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

      camera.w = bobTheta;
      camera.align();

      camera.warpMoveZ( Point3( bot->p.x, bot->p.y, bot->p.z + bot->camZ + bobBias ) );
    }
  }
  else { // external
    bobPhi   = 0.0f;
    bobTheta = 0.0f;
    bobBias  = 0.0f;

    camera.w = 0.0f;
    camera.align();

    float dist;
    if( bot->parent != -1 && orbis.objects[bot->parent] ) {
      Vehicle* veh = static_cast<Vehicle*>( orbis.objects[bot->parent] );

      hard_assert( veh->flags & Object::VEHICLE_BIT );

      dist = veh->dim.fastL() * THIRD_PERSON_DIST;
    }
    else {
      dist = bot->dim.fastL() * THIRD_PERSON_DIST;
    }

    Point3 origin = bot->p + Vec3( 0.0f, 0.0f, bot->camZ );
    Vec3   offset = -camera.at * dist;

    collider.translate( origin, offset, bot );

    offset *= collider.hit.ratio;
    offset += camera.at * THIRD_PERSON_CLIP_DIST;

    camera.warpMoveZ( origin + offset );
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
    Point3 p        = camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    Vec3   at       = Vec3( -hvsc[4], hvsc[5], -hvsc[3] );
    float  distance = static_cast<const BotClass*>( camera.botObj->clazz )->reachDist;

    collider.mask = ~0;
    collider.translate( p, at * distance, camera.botObj );
    collider.mask = Object::SOLID_BIT;

    camera.setTagged( collider.hit.obj );
  }
  else {
    Point3 p        = camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    float  distance = static_cast<const BotClass*>( camera.botObj->clazz )->reachDist;

    collider.mask = ~0;
    collider.translate( p, camera.at * distance, camera.botObj );
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

void BotProxy::write( BufferStream* ostream ) const
{
  ostream->writeBool( isExternal );
  ostream->writeBool( isFreelook );
}

void BotProxy::init()
{}

}
}
