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
 * @file client/UnitProxy.cc
 */

#include "stable.hh"

#include "client/UnitProxy.hh"

#include "client/Camera.hh"

#include "client/ui/GalileoFrame.hh"
#include "client/ui/UI.hh"

namespace oz
{
namespace client
{

const float UnitProxy::CAMERA_Z_SMOOTHING       = 0.35f;
const float UnitProxy::CAMERA_Z_TOLERANCE       = 0.20f;
const float UnitProxy::EXTERNAL_CAM_DIST        = 2.75f;
const float UnitProxy::EXTERNAL_CAM_CLIP_DIST   = 0.10f;
const float UnitProxy::SHOULDER_CAM_RIGHT       = 0.25f;
const float UnitProxy::SHOULDER_CAM_UP          = 0.25f;
const float UnitProxy::VEHICLE_CAM_UP_FACTOR    = 0.15f;
const float UnitProxy::BOB_SUPPRESSION_COEF     = 0.80f;
const float UnitProxy::BINOCULARS_MAGNIFICATION = 0.20f;

UnitProxy::UnitProxy() :
  hud( null ), infoFrame( null ), inventory( null ), container( null )
{}

void UnitProxy::begin()
{
  if( camera.bot < 0 ) {
    return;
  }

  Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

  camera.setTaggedObj( null );
  camera.setTaggedEnt( null );
  camera.isExternal = isExternal;

  hud       = new ui::HudArea();
  infoFrame = new ui::InfoFrame();
  inventory = new ui::InventoryMenu( null );
  container = new ui::InventoryMenu( inventory );

  ui::mouse.doShow = false;

  ui::ui.galileoFrame->show( true );
  ui::ui.root->add( hud );
  ui::ui.root->add( infoFrame );
  ui::ui.root->add( inventory );
  ui::ui.root->add( container );

  hud->sink();
  inventory->show( false );
  container->show( false );

  baseRot   = Quat::rotationZXZ( bot->h, bot->v, 0.0f );
  headRot   = Quat::ID;
  headH     = 0.0f;
  headV     = 0.0f;

  botEye    = bot->p;
  botEye.z += bot->camZ;

  bobTheta  = 0.0f;
  bobBias   = 0.0f;
}

void UnitProxy::end()
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

void UnitProxy::prepare()
{
  if( camera.bot < 0 ) {
    return;
  }

  const ubyte* keys    = ui::keyboard.keys;
  const ubyte* oldKeys = ui::keyboard.oldKeys;

  bool alt = keys[SDLK_LALT] || keys[SDLK_RALT];

  Bot*     bot = camera.botObj;
  Vehicle* veh = camera.vehicleObj;

  bot->actions = 0;

  /*
   * Camera
   */

  if( !isFreelook || ( veh == null && !isExternal ) ) {
    bot->h += camera.relH;
    bot->v += camera.relV;
  }

  if( keys[SDLK_q] ) {
    bot->h += camera.keyXSens;
  }
  if( keys[SDLK_e] ) {
    bot->h -= camera.keyXSens;
  }

  bot->h = angleWrap( bot->h );
  bot->v = clamp( bot->v, 0.0f, Math::TAU / 2.0f );

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
    bot->actions |= Bot::ACTION_WALK;
  }

  if( !alt && keys[SDLK_x] && !oldKeys[SDLK_x] ) {
    bot->actions |= Bot::ACTION_EXIT;
  }
  if( alt && keys[SDLK_x] && !oldKeys[SDLK_x] ) {
    bot->actions |= Bot::ACTION_EJECT;
  }
  if( alt && keys[SDLK_k] && !oldKeys[SDLK_k] ) {
    if( bot->hasAttribute( ObjectClass::SUICIDE_BIT ) ) {
      bot->actions |= Bot::ACTION_SUICIDE;
    }
  }

  if( !alt && keys[SDLK_f] ) {
    bot->actions |= Bot::ACTION_POINT;
  }
  if( !alt && keys[SDLK_g] ) {
    bot->actions |= Bot::ACTION_BACK;
  }
  if( !alt && keys[SDLK_h] ) {
    bot->actions |= Bot::ACTION_SALUTE;
  }
  if( !alt && keys[SDLK_j] ) {
    bot->actions |= Bot::ACTION_WAVE;
  }
  if( !alt && keys[SDLK_k] ) {
    bot->actions |= Bot::ACTION_FLIP;
  }

  /*
   * View
   */

  if( !alt && keys[SDLK_n] && !oldKeys[SDLK_n] ) {
    camera.nightVision = !camera.nightVision;
  }
  if( !alt && keys[SDLK_b] && !oldKeys[SDLK_b] ) {
    camera.mag = camera.mag == 1.0f ? BINOCULARS_MAGNIFICATION : 1.0f;
  }
  if( !alt && keys[SDLK_m] && !oldKeys[SDLK_m] ) {
    ui::ui.galileoFrame->setMaximised( !ui::ui.galileoFrame->isMaximised );
  }

  if( camera.nightVision && !bot->hasAttribute( ObjectClass::NIGHT_VISION_BIT ) ) {
    camera.nightVision = false;
  }
  if( camera.mag != 1.0f && !bot->hasAttribute( ObjectClass::BINOCULARS_BIT ) ) {
    camera.mag = 1.0f;
  }

  if( !alt && keys[SDLK_KP_ENTER] && !oldKeys[SDLK_KP_ENTER] ) {
    isExternal = !isExternal;
    camera.isExternal = isExternal;

    if( isExternal && isFreelook ) {
      headH = bot->h;
      headV = bot->v;
    }
    else {
      headH = 0.0f;
      headV = 0.0f;
    }
  }
  if( !alt && keys[SDLK_KP_MULTIPLY] && !oldKeys[SDLK_KP_MULTIPLY] ) {
    isFreelook = !isFreelook;

    if( isExternal && isFreelook ) {
      headH = bot->h;
      headV = bot->v;
    }
    else {
      headH = 0.0f;
      headV = 0.0f;
    }
  }

  /*
   * Mouse
   */

  if( !ui::mouse.doShow ) {
    if( ui::mouse.buttons & SDL_BUTTON_LMASK ) {
      bot->actions |= Bot::ACTION_ATTACK;
    }

    if( ui::mouse.leftClick ) {
      if( bot->cargo >= 0 ) {
        bot->rotateCargo();
      }
    }
    if( ui::mouse.rightClick ) {
      if( bot->parent >= 0 ) {
        bot->actions |= Bot::ACTION_VEH_NEXT_WEAPON;
      }
      else if( camera.entityObj != null ) {
        bot->trigger( camera.entityObj );
      }
      else if( camera.objectObj != null ) {
        bot->use( camera.objectObj );
      }
    }
    else if( ui::mouse.middleClick ) {
      if( bot->cargo >= 0 ) {
        bot->grab();
      }
      else if( camera.entity >= 0 ) {
        bot->lock( camera.entityObj );
      }
      else if( camera.object >= 0 ) {
        Dynamic* dyn = static_cast<Dynamic*>( const_cast<Object*>( camera.objectObj ) );

        if( dyn->flags & Object::DYNAMIC_BIT ) {
          bot->grab( dyn );
        }
      }
    }
    else if( ui::mouse.wheelDown ) {
      if( camera.objectObj != null ) {
        if( camera.objectObj->flags & Object::BROWSABLE_BIT ) {
          ui::mouse.doShow = true;
        }
        else {
          Dynamic* dyn = static_cast<Dynamic*>( const_cast<Object*>( camera.objectObj ) );

          if( dyn->flags & Object::DYNAMIC_BIT ) {
            bot->take( dyn );
          }
        }
      }
    }
    else if( ui::mouse.wheelUp ) {
      if( bot->cargo >= 0 ) {
        bot->throwCargo();
      }
    }
  }

  /*
   * Other
   */

  if( !alt && keys[SDLK_i] && !oldKeys[SDLK_i] ) {
    if( camera.allowReincarnation ) {
      bot->actions = 0;
      camera.setBot( null );
      return;
    }
  }

  if( !alt && keys[SDLK_y] && !oldKeys[SDLK_y] ) {
    int nSwitchableunits = camera.switchableUnits.length();

    if( nSwitchableunits != 0 ) {
      int currUnit = -1;

      for( int i = 0; i < nSwitchableunits; ++i ) {
        if( camera.bot == camera.switchableUnits[i] ) {
          currUnit = i;
          break;
        }
      }

      currUnit = ( currUnit + 1 ) % nSwitchableunits;

      Bot* unit = static_cast<Bot*>( orbis.objects[ camera.switchableUnits[currUnit] ] );

      bot->actions = 0;
      camera.setBot( unit );
    }
  }

  if( !alt && keys[SDLK_o] ) {
    if( keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT] ) {
      orbis.caelum.time -= 0.1f * Timer::TICK_TIME * orbis.caelum.period;
    }
    else {
      orbis.caelum.time += 0.1f * Timer::TICK_TIME * orbis.caelum.period;
    }
  }

  if( keys[SDLK_TAB] && !oldKeys[SDLK_TAB] ) {
    ui::mouse.doShow = !ui::mouse.doShow;
  }
}

void UnitProxy::update()
{
  if( camera.bot < 0 ) {
    camera.setState( Camera::STRATEGIC );
    return;
  }

  const Bot*          bot      = camera.botObj;
  const BotClass*     botClazz = static_cast<const BotClass*>( bot->clazz );
  const Vehicle*      veh      = camera.vehicleObj;
  const VehicleClass* vehClazz = null;

  if( veh != null ) {
    vehClazz = static_cast<const VehicleClass*>( veh->clazz );

    // Rotation for -90°, faster than matrix multiplication.
    Mat44 rotMat = veh->rot;
    rotMat.rotateX( Math::TAU / -4.0f );

    botEye = veh->p + rotMat * vehClazz->pilotPos;
  }
  else {
    float actualZ = bot->p.z + bot->camZ;

    botEye.x = bot->p.x;
    botEye.y = bot->p.y;
    botEye.z = Math::mix( botEye.z, actualZ, CAMERA_Z_SMOOTHING );
    botEye.z = clamp( botEye.z, actualZ - CAMERA_Z_TOLERANCE, actualZ + CAMERA_Z_TOLERANCE );
  }

  // external
  if( isExternal ) {
    if( isFreelook ) {
      headH = angleWrap( headH + camera.relH );
      headV = clamp( headV + camera.relV, 0.0f, +Math::TAU / 2.0f );

      Quat completeRot = Quat::rotationZXZ( headH, headV, 0.0f );

      baseRot = Quat::rotationZXZ( bot->h, bot->v, 0.0f );
      headRot = *baseRot * completeRot;

      camera.smoothRotateTo( completeRot );
    }
    else {
      headH = 0.0f;
      headV = 0.0f;

      baseRot = Quat::rotationZXZ( bot->h, bot->v, 0.0f );
      headRot = Quat::ID;

      camera.smoothRotateTo( baseRot );
    }

    bobTheta = 0.0f;
    bobBias  = 0.0f;

    camera.align();

    Vec3 offset;

    if( veh != null ) {
      float dist = veh->dim.fastN() * EXTERNAL_CAM_DIST;
      offset = camera.rotMat * Vec3( 0.0f, VEHICLE_CAM_UP_FACTOR * dist, dist );
    }
    else {
      float dist = bot->dim.fastN() * EXTERNAL_CAM_DIST;
      offset = camera.rotMat * Vec3( SHOULDER_CAM_RIGHT, SHOULDER_CAM_UP, dist );
    }

    collider.translate( botEye, offset, bot );
    offset *= collider.hit.ratio;

    float dist = !offset;
    if( dist > EXTERNAL_CAM_CLIP_DIST ) {
      offset *= ( dist - EXTERNAL_CAM_CLIP_DIST ) / dist;
    }
    else {
      offset = Vec3::ZERO;
    }

    camera.moveTo( botEye + offset );
  }
  else {
    // internal, vehicle
    if( veh != null ) {
      if( isFreelook ) {
        headH = clamp( headH + camera.relH, vehClazz->lookHMin, vehClazz->lookHMax );
        headV = clamp( headV + camera.relV, vehClazz->lookVMin, vehClazz->lookVMax );
      }
      else {
        headH = 0.0f;
        headV = 0.0f;
      }

      bobTheta = 0.0f;
      bobBias  = 0.0f;

      baseRot = Quat::rotationZXZ( veh->h, veh->v, veh->w );
      headRot = Quat::rotationY( headH ) * Quat::rotationX( headV );

      camera.rotateTo( baseRot * headRot );
      camera.align();
      camera.moveTo( botEye );
    }
    // internal, bot
    else {
      if( ( bot->state & ( Bot::MOVING_BIT | Bot::SWIMMING_BIT | Bot::CLIMBING_BIT ) ) ==
          Bot::MOVING_BIT )
      {
        float phase = bot->step * Math::TAU;
        float sine  = Math::sin( phase );

        bobTheta = sine * botClazz->bobRotation;
        bobBias  = sine*sine * botClazz->bobAmplitude;
      }
      else if( ( bot->state & ( Bot::MOVING_BIT | Bot::SWIMMING_BIT | Bot::CLIMBING_BIT ) ) ==
               ( Bot::MOVING_BIT | Bot::SWIMMING_BIT ) )
      {
        float sine = Math::sin( bot->step * Math::TAU / 2.0f );

        bobTheta = 0.0f;
        bobBias  = sine*sine * botClazz->bobSwimAmplitude;
      }
      else {
        bobTheta *= BOB_SUPPRESSION_COEF;
        bobBias  *= BOB_SUPPRESSION_COEF;
      }

      baseRot = Quat::rotationZXZ( bot->h, bot->v, bobTheta );
      headRot = Quat::ID;
      headH   = 0.0f;
      headV   = 0.0f;

      camera.rotateTo( baseRot );
      camera.align();
      camera.moveTo( Point( botEye.x, botEye.y, botEye.z + bobBias ) );
    }
  }

  if( bot->parent >= 0 ) {
    camera.setTaggedObj( null );
  }
  else if( bot->cargo >= 0 ) {
    camera.setTaggedObj( orbis.objects[bot->cargo] );
  }
  else {
    // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
    float hvsc[6];

    Math::sincos( bot->h, &hvsc[0], &hvsc[1] );
    Math::sincos( bot->v, &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[2] * hvsc[0];
    hvsc[5] = hvsc[2] * hvsc[1];

    Vec3  at    = Vec3( -hvsc[4], +hvsc[5], -hvsc[3] );
    Point eye   = bot->p + Vec3( 0.0f, 0.0f, bot->camZ );
    Vec3  reach = at * botClazz->reachDist;

    collider.mask = ~0;
    collider.translate( eye, reach, bot );
    collider.mask = Object::SOLID_BIT;

    camera.setTaggedObj( collider.hit.obj );
    camera.setTaggedEnt( collider.hit.entity );
  }
}

void UnitProxy::reset()
{
  baseRot    = Quat::ID;
  headRot    = Quat::ID;
  headH      = 0.0f;
  headV      = 0.0f;

  bobTheta   = 0.0f;
  bobBias    = 0.0f;

  isExternal = false;
  isFreelook = false;
}

void UnitProxy::read( InputStream* istream )
{
  baseRot    = Quat::ID;
  headRot    = Quat::ID;
  headH      = 0.0f;
  headV      = 0.0f;

  bobTheta   = 0.0f;
  bobBias    = 0.0f;

  isExternal = istream->readBool();
  isFreelook = istream->readBool();
}

void UnitProxy::write( BufferStream* ostream ) const
{
  ostream->writeBool( isExternal );
  ostream->writeBool( isFreelook );
}

}
}
