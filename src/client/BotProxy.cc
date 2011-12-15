/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
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
 */

/**
 * @file client/BotProxy.cc
 */

#include "stable.hh"

#include "client/BotProxy.hh"

#include "matrix/Collider.hh"

#include "client/Camera.hh"
#include "client/ui/UI.hh"

namespace oz
{
namespace client
{

const float BotProxy::EXTERNAL_CAM_DIST      = 2.75f;
const float BotProxy::EXTERNAL_CAM_CLIP_DIST = 0.20f;
const float BotProxy::SHOULDER_CAM_RIGHT     = 0.25f;
const float BotProxy::SHOULDER_CAM_UP        = 0.25f;
const float BotProxy::VEHICLE_CAM_UP_FACTOR  = 0.15f;
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

  ui::ui.root->sink( container );
  ui::ui.root->sink( inventory );
  ui::ui.root->sink( infoFrame );
  ui::ui.root->sink( hud );

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
  if( isFreelook && ( bot->parent != -1 || isExternal ) ) {
    camera.h += camera.relH;
    camera.v += camera.relV;
  }
  else {
    bot->h = Math::fmod( bot->h + camera.relH + 2.0f*Math::TAU, Math::TAU );
    bot->v = clamp( bot->v + camera.relV, 0.0f, Math::TAU / 2.0f );

    camera.h = bot->h;
    camera.v = bot->v;
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
  if( keys[SDLK_KP_MULTIPLY] && !oldKeys[SDLK_KP_MULTIPLY] && isExternal ) {
    isFreelook = !isFreelook;

    camera.h = bot->h;
    camera.v = bot->v;
  }

  if( keys[SDLK_f] ) {
    bot->actions |= Bot::ACTION_GESTURE0;
  }
  if( keys[SDLK_g] ) {
    bot->actions |= Bot::ACTION_GESTURE1;
  }
  if( keys[SDLK_h] ) {
    bot->actions |= Bot::ACTION_GESTURE2;
  }
  if( keys[SDLK_j] ) {
    bot->actions |= Bot::ACTION_GESTURE3;
  }
  if( keys[SDLK_k] ) {
    bot->actions |= Bot::ACTION_GESTURE4;
  }

  if( ui::keyboard.keys[SDLK_TAB] && !ui::keyboard.oldKeys[SDLK_TAB] ) {
    ui::mouse.doShow = !ui::mouse.doShow;

    inventory->show( ui::mouse.doShow );
    container->show( true );
  }

  if( !ui::mouse.doShow ) {
    if( ui::mouse.buttons & SDL_BUTTON_LMASK ) {
      bot->actions |= Bot::ACTION_ATTACK;
    }
    if( ui::mouse.rightClick ) {
      if( bot->parent != -1 ) {
        bot->actions |= Bot::ACTION_VEH_NEXT_WEAPON;
      }
      else if( camera.tagged != -1 ) {
        bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
        bot->actions |= Bot::ACTION_USE;
        bot->instrument = camera.tagged;
        bot->container = -1;
      }
    }
    else if( ui::mouse.wheelDown ) {
      if( camera.taggedObj != null ) {
        if( camera.taggedObj->flags & Object::BROWSABLE_BIT ) {
          ui::mouse.doShow = true;

          inventory->show( true );
          container->show( true );
        }
        else {
          bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
          bot->actions |= Bot::ACTION_TAKE;
          bot->instrument = camera.tagged;
          bot->container = -1;
        }
      }
    }
    else if( ui::mouse.wheelUp ) {
      if( bot->cargo != -1 ) {
        bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
        bot->actions |= Bot::ACTION_THROW;
        bot->instrument = -1;
        bot->container = -1;
      }
    }
    else if( ui::mouse.middleClick ) {
      if( bot->cargo != -1 ) {
        bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
        bot->actions |= Bot::ACTION_GRAB;
        bot->instrument = -1;
        bot->container = -1;
      }
      else if( camera.tagged != -1 ) {
        bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
        bot->actions |= Bot::ACTION_GRAB;
        bot->instrument = camera.tagged;
        bot->container = -1;
      }
    }
    else if( keys[SDLK_q] && !oldKeys[SDLK_q] ) {
      if( bot->cargo != -1 ) {
        bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
        bot->actions |= Bot::ACTION_ROTATE;
        bot->instrument = -1;
        bot->container = -1;
      }
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

  if( !isExternal ) {
    if( !isFreelook ) {
      camera.h = bot->h;
      camera.v = bot->v;
    }

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
            ( bot->state & ( Bot::RUNNING_BIT | Bot::CROUCHING_BIT | Bot::CARGO_BIT ) ) ==
              Bot::RUNNING_BIT ? clazz->bobRunInc : clazz->bobWalkInc;

        bobPhi   = Math::fmod( bobPhi + bobInc, Math::TAU );
        bobTheta = Math::sin( bobPhi ) * clazz->bobRotation;
        bobBias  = Math::sin( 2.0f * bobPhi ) * clazz->bobAmplitude;
      }
      else if( ( bot->state & ( Bot::MOVING_BIT | Bot::SWIMMING_BIT | Bot::CLIMBING_BIT ) ) ==
          ( Bot::MOVING_BIT | Bot::SWIMMING_BIT ) )
      {
        float bobInc =
            ( bot->state & ( Bot::RUNNING_BIT | Bot::CROUCHING_BIT ) ) == Bot::RUNNING_BIT ?
            clazz->bobSwimRunInc : clazz->bobSwimInc;

        bobPhi   = Math::fmod( bobPhi + bobInc, Math::TAU );
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

    if( !isFreelook ) {
      camera.h = bot->h;
      camera.v = bot->v;
    }

    camera.w = 0.0f;
    camera.align();

    Point3 origin = Point3( bot->p.x, bot->p.y, bot->p.z + bot->camZ );
    Vec3   offset;

    if( bot->parent != -1 && orbis.objects[bot->parent] ) {
      Vehicle* veh = static_cast<Vehicle*>( orbis.objects[bot->parent] );

      hard_assert( veh->flags & Object::VEHICLE_BIT );

      float dist = veh->dim.fastL() * EXTERNAL_CAM_DIST;
      offset = camera.rotMat * Vec3( 0.0f, VEHICLE_CAM_UP_FACTOR * dist, dist );
    }
    else {
      float dist = bot->dim.fastL() * EXTERNAL_CAM_DIST;
      offset = camera.rotMat * Vec3( SHOULDER_CAM_RIGHT, SHOULDER_CAM_UP, dist );
    }

    collider.translate( origin, offset, bot );
    camera.warpMoveZ( origin + offset * collider.hit.ratio );
  }

  if( bot->parent != -1 ) {
    camera.setTagged( null );
  }
  else if( bot->cargo != -1 ) {
    camera.setTagged( orbis.objects[bot->cargo] );
  }
  else {
    camera.setTagged( bot->getTagged( ~0 ) );
  }
}

void BotProxy::reset()
{
  isExternal = false;
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
