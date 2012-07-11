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
#include "client/Input.hh"

#include "client/ui/HudArea.hh"
#include "client/ui/InfoFrame.hh"
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

void UnitProxy::begin()
{
  if( camera.bot < 0 ) {
    return;
  }

  Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

  camera.setTaggedObj( null );
  camera.setTaggedEnt( null );
  camera.isExternal = isExternal;

  ui::mouse.doShow = false;

  ui::ui.hudArea->show( true );
  ui::ui.infoFrame->show( true );
  ui::ui.galileoFrame->show( true );

  baseRot = Quat::rotationZXZ( bot->h, bot->v, 0.0f );
  headRot = Quat::ID;

  if( isFreelook ) {
    headH = bot->h;
    headV = bot->v;
  }
  else {
    headH = 0.0f;
    headV = 0.0f;
  }

  botEye    = bot->p;
  botEye.z += bot->camZ;
  bobTheta  = 0.0f;
  bobBias   = 0.0f;

  oldBot    = -1;
}

void UnitProxy::end()
{
  ui::mouse.doShow = true;

  ui::ui.galileoFrame->show( false );
  ui::ui.infoFrame->show( false );
  ui::ui.hudArea->show( false );
}

void UnitProxy::prepare()
{
  if( camera.bot < 0 ) {
    return;
  }

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

  if( input.keys[Input::KEY_TURN_LEFT] ) {
    bot->h += input.keySensH;
  }
  if( input.keys[Input::KEY_TURN_RIGHT] ) {
    bot->h -= input.keySensV;
  }

  bot->h = angleWrap( bot->h );
  bot->v = clamp( bot->v, 0.0f, Math::TAU / 2.0f );

  /*
   * Movement
   */

  if( input.keys[Input::KEY_MOVE_FORWARD] ) {
    bot->actions |= Bot::ACTION_FORWARD;
  }
  if( input.keys[Input::KEY_MOVE_BACKWARD] ) {
    bot->actions |= Bot::ACTION_BACKWARD;
  }
  if( input.keys[Input::KEY_MOVE_RIGHT] ) {
    bot->actions |= Bot::ACTION_RIGHT;
  }
  if( input.keys[Input::KEY_MOVE_LEFT] ) {
    bot->actions |= Bot::ACTION_LEFT;
  }

  /*
   * Actions
   */

  if( input.keys[Input::KEY_JUMP] ) {
    bot->actions |= Bot::ACTION_JUMP;
  }
  if( input.keys[Input::KEY_CROUCH_TOGGLE] && !input.oldKeys[Input::KEY_CROUCH_TOGGLE] ) {
    bot->actions |= Bot::ACTION_CROUCH;
  }
  if( input.keys[Input::KEY_SPEED_TOGGLE] && !input.oldKeys[Input::KEY_SPEED_TOGGLE] ) {
    bot->actions |= Bot::ACTION_WALK;
  }

  if( input.keys[Input::KEY_MOVE_UP] ) {
    bot->actions |= Bot::ACTION_VEH_UP;
  }
  if( input.keys[Input::KEY_MOVE_DOWN] ) {
    bot->actions |= Bot::ACTION_VEH_DOWN;
  }

  if( input.keys[Input::KEY_EXIT] && !input.oldKeys[Input::KEY_EXIT] ) {
    bot->actions |= Bot::ACTION_EXIT;
  }
  if( input.keys[Input::KEY_EJECT] && !input.oldKeys[Input::KEY_EJECT] ) {
    bot->actions |= Bot::ACTION_EJECT;
  }
  if( input.keys[Input::KEY_SUICIDE] && !input.oldKeys[Input::KEY_SUICIDE] ) {
    if( bot->hasAttribute( ObjectClass::SUICIDE_BIT ) ) {
      bot->actions |= Bot::ACTION_SUICIDE;
    }
  }

  if( input.keys[Input::KEY_GESTURE_POINT] ) {
    bot->actions |= Bot::ACTION_POINT;
  }
  if( input.keys[Input::KEY_GESTURE_BACK] ) {
    bot->actions |= Bot::ACTION_BACK;
  }
  if( input.keys[Input::KEY_GESTURE_SALUTE] ) {
    bot->actions |= Bot::ACTION_SALUTE;
  }
  if( input.keys[Input::KEY_GESTURE_WAVE] ) {
    bot->actions |= Bot::ACTION_WAVE;
  }
  if( input.keys[Input::KEY_GESTURE_FLIP] ) {
    bot->actions |= Bot::ACTION_FLIP;
  }

  /*
   * View
   */

  if( input.keys[Input::KEY_NV_TOGGLE] && !input.oldKeys[Input::KEY_NV_TOGGLE] ) {
    camera.nightVision = !camera.nightVision;
  }

  if( input.keys[Input::KEY_BINOCULARS_TOGGLE] && !input.oldKeys[Input::KEY_BINOCULARS_TOGGLE] ) {
    if( camera.desiredMag == 1.0f ) {
      camera.smoothMagnify( BINOCULARS_MAGNIFICATION );
    }
    else {
      camera.smoothMagnify( 1.0f );
    }
  }

  if( input.keys[Input::KEY_MAP_TOGGLE] && !input.oldKeys[Input::KEY_MAP_TOGGLE] ) {
    ui::ui.galileoFrame->setMaximised( !ui::ui.galileoFrame->isMaximised );
  }

  if( camera.nightVision && !bot->hasAttribute( ObjectClass::NIGHT_VISION_BIT ) ) {
    camera.nightVision = false;
  }

  if( camera.desiredMag != 1.0f && !bot->hasAttribute( ObjectClass::BINOCULARS_BIT ) ) {
    camera.desiredMag = 1.0f;
  }

  if( input.keys[Input::KEY_CAMERA_TOGGLE] && !input.oldKeys[Input::KEY_CAMERA_TOGGLE] ) {
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
  if( input.keys[Input::KEY_FREELOOK_TOGGLE] && !input.oldKeys[Input::KEY_FREELOOK_TOGGLE] ) {
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
    if( input.buttons & SDL_BUTTON_LMASK ) {
      bot->actions |= Bot::ACTION_ATTACK;
    }

    if( input.leftClick ) {
      if( bot->cargo >= 0 ) {
        bot->rotateCargo();
      }
    }
    if( input.rightClick ) {
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
    else if( input.middleClick ) {
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
    else if( input.wheelDown ) {
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
    else if( input.wheelUp ) {
      if( bot->cargo >= 0 ) {
        bot->throwCargo();
      }
    }
  }

  /*
   * Other
   */

  if( input.keys[Input::KEY_SWITCH_TO_UNIT] && !input.oldKeys[Input::KEY_SWITCH_TO_UNIT] ) {
    if( camera.allowReincarnation ) {
      bot->actions = 0;
      camera.setBot( null );
      return;
    }
  }

  if( input.keys[Input::KEY_CYCLE_UNITS] && !input.oldKeys[Input::KEY_CYCLE_UNITS] ) {
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

    if( oldBot != camera.bot ) {
      botEye.z = actualZ;
    }
    else {
      botEye.z = Math::mix( botEye.z, actualZ, CAMERA_Z_SMOOTHING );
      botEye.z = clamp( botEye.z, actualZ - CAMERA_Z_TOLERANCE, actualZ + CAMERA_Z_TOLERANCE );
    }
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

  oldBot = camera.bot;
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
