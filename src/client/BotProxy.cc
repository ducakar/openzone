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
 * @file client/BotProxy.cc
 */

#include "stable.hh"

#include "client/BotProxy.hh"

#include "client/Camera.hh"
#include "client/ui/UI.hh"

namespace oz
{
namespace client
{

const float BotProxy::EXTERNAL_CAM_DIST        = 2.75f;
const float BotProxy::EXTERNAL_CAM_CLIP_DIST   = 0.10f;
const float BotProxy::SHOULDER_CAM_RIGHT       = 0.25f;
const float BotProxy::SHOULDER_CAM_UP          = 0.25f;
const float BotProxy::VEHICLE_CAM_UP_FACTOR    = 0.15f;
const float BotProxy::BOB_SUPPRESSION_COEF     = 0.80f;
const float BotProxy::BINOCULARS_MAGNIFICATION = 0.20f;

BotProxy::BotProxy() :
  hud( null ), infoFrame( null ), inventory( null ), container( null )
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
  camera.setTaggedObj( null );
  camera.setTaggedEnt( null );

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

void BotProxy::prepare()
{
  if( camera.bot == -1 ) {
    return;
  }

  Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

  const ubyte* keys    = ui::keyboard.keys;
  const ubyte* oldKeys = ui::keyboard.oldKeys;

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

  if( keys[SDLK_o] ) {
    if( keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT] ) {
      orbis.caelum.time -= 0.1f * Timer::TICK_TIME * orbis.caelum.period;
    }
    else {
      orbis.caelum.time += 0.1f * Timer::TICK_TIME * orbis.caelum.period;
    }
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
  if( ( keys[SDLK_LALT] || keys[SDLK_RALT] ) && keys[SDLK_k] && !oldKeys[SDLK_k] ) {
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

  if( keys[SDLK_n] && !oldKeys[SDLK_n] ) {
    camera.nightVision = !camera.nightVision;
  }
  if( keys[SDLK_b] && !oldKeys[SDLK_b] ) {
    camera.mag = camera.mag == 1.0f ? BINOCULARS_MAGNIFICATION : 1.0f;
  }

  if( camera.nightVision && !bot->hasAttribute( ObjectClass::NIGHT_VISION_BIT ) ) {
    camera.nightVision = false;
  }
  if( camera.mag != 1.0f && !bot->hasAttribute( ObjectClass::BINOCULARS_BIT ) ) {
    camera.mag = 1.0f;
  }

  if( keys[SDLK_TAB] && !oldKeys[SDLK_TAB] ) {
    ui::mouse.doShow = !ui::mouse.doShow;
  }

  if( !ui::mouse.doShow ) {
    if( ui::mouse.buttons & SDL_BUTTON_LMASK ) {
      bot->actions |= Bot::ACTION_ATTACK;
    }

    if( ui::mouse.leftClick ) {
      if( bot->cargo != -1 ) {
        bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
        bot->actions |= Bot::ACTION_ROTATE;
        bot->instrument = -1;
        bot->container = -1;
        bot->trigger = -1;
      }
    }
    if( ui::mouse.rightClick ) {
      if( bot->parent != -1 ) {
        bot->actions |= Bot::ACTION_VEH_NEXT_WEAPON;
      }
      else if( camera.object != -1 ) {
        if( camera.objectObj->flags & Object::USE_FUNC_BIT ) {
          bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
          bot->actions |= Bot::ACTION_USE;
          bot->instrument = camera.object;
          bot->container = -1;
          bot->trigger = -1;
        }
      }
      else if( camera.entity != -1 ) {
        int targetEntity = camera.entityObj->model->target;

        if( targetEntity != -1 ) {
          bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
          bot->actions |= Bot::ACTION_USE;
          bot->instrument = -1;
          bot->container = -1;
          bot->trigger = camera.entity;
        }
      }
    }
    else if( ui::mouse.middleClick ) {
      if( bot->cargo != -1 ) {
        bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
        bot->actions |= Bot::ACTION_GRAB;
        bot->instrument = -1;
        bot->container = -1;
        bot->trigger = -1;
      }
      else if( camera.entity != -1 ) {
        bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
        bot->actions |= Bot::ACTION_LOCK;
        bot->instrument = -1;
        bot->container = -1;
        bot->trigger = camera.entity;
      }
      else if( camera.object != -1 ) {
        bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
        bot->actions |= Bot::ACTION_GRAB;
        bot->instrument = camera.object;
        bot->container = -1;
        bot->trigger = -1;
      }
    }
    else if( ui::mouse.wheelDown ) {
      if( camera.objectObj != null ) {
        if( camera.objectObj->flags & Object::BROWSABLE_BIT ) {
          ui::mouse.doShow = true;

          inventory->show( true );
          container->show( true );
        }
        else {
          bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
          bot->actions |= Bot::ACTION_TAKE;
          bot->instrument = camera.object;
          bot->container = -1;
          bot->trigger = -1;
        }
      }
    }
    else if( ui::mouse.wheelUp ) {
      if( bot->cargo != -1 ) {
        bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
        bot->actions |= Bot::ACTION_THROW;
        bot->instrument = -1;
        bot->container = -1;
        bot->trigger = -1;
      }
    }
  }
}

void BotProxy::update()
{
  if( camera.bot == -1 ) {
    camera.setState( Camera::STRATEGIC );
    return;
  }

  const Bot*      bot   = camera.botObj;
  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

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
      if( ( bot->actions & Bot::ACTION_JUMP ) &&
          !( bot->state & ( Bot::GROUNDED_BIT | Bot::CLIMBING_BIT ) ) )
      {
        goto inJump;
      }

      if( ( bot->state & ( Bot::MOVING_BIT | Bot::SWIMMING_BIT | Bot::CLIMBING_BIT ) ) ==
          Bot::MOVING_BIT )
      {
        float sine = Math::sin( bobPhi );
        float tilt = Math::sin( bobPhi + Math::TAU / 4.0f ) * clazz->bobRotation;

        bobTheta = Math::mix( bobTheta, tilt, 0.35f );
        bobBias  = sine*sine * clazz->bobAmplitude;
      }
      else if( ( bot->state & ( Bot::MOVING_BIT | Bot::SWIMMING_BIT | Bot::CLIMBING_BIT ) ) ==
               ( Bot::MOVING_BIT | Bot::SWIMMING_BIT ) )
      {
        float sine = Math::sin( bobPhi / 2.0f );

        bobTheta = 0.0f;
        bobBias  = sine*sine * clazz->bobSwimAmplitude;
      }
      else {
      inJump:;
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
    if( !isFreelook ) {
      camera.h = bot->h;
      camera.v = bot->v;
    }

    bobTheta = 0.0f;
    bobBias  = 0.0f;

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
    offset *= collider.hit.ratio;

    float dist = !offset;
    if( dist > EXTERNAL_CAM_CLIP_DIST ) {
      offset *= ( dist - EXTERNAL_CAM_CLIP_DIST ) / dist;
    }
    else {
      offset = Vec3::ZERO;
    }

    camera.warpMoveZ( origin + offset );
  }

  if( bot->parent != -1 ) {
    camera.setTaggedObj( null );
  }
  else if( bot->cargo != -1 ) {
    camera.setTaggedObj( orbis.objects[bot->cargo] );
  }
  else {
    // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
    float hvsc[6];

    Math::sincos( bot->h, &hvsc[0], &hvsc[1] );
    Math::sincos( bot->v, &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[2] * hvsc[0];
    hvsc[5] = hvsc[2] * hvsc[1];

    Vec3   at    = Vec3( -hvsc[4], hvsc[5], -hvsc[3] );
    Point3 eye   = bot->p + Vec3( 0.0f, 0.0f, bot->camZ );
    Vec3   reach = at * clazz->reachDist;

    collider.mask = ~0;
    collider.translate( eye, reach, bot );
    collider.mask = Object::SOLID_BIT;

    camera.setTaggedObj( collider.hit.obj );
    camera.setTaggedEnt( collider.hit.entity );
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
