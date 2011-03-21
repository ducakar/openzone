/*
 *  BotProxy.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/BotProxy.hpp"

#include "matrix/Collider.hpp"
#include "matrix/BotClass.hpp"
#include "matrix/VehicleClass.hpp"

#include "ui/Keyboard.hpp"
#include "ui/Mouse.hpp"

#include "client/Camera.hpp"

namespace oz
{
namespace client
{

  const float BotProxy::THIRD_PERSON_CLIP_DIST = 0.20f;
  const float BotProxy::BOB_SUPPRESSION_COEF   = 0.80f;

  void BotProxy::begin()
  {
    hard_assert( camera.bot != -1 );

    Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

    ui::mouse.doShow = false;

    bobPhi   = 0.0f;
    bobTheta = 0.0f;
    bobBias  = 0.0f;

    camera.h = bot->h;
    camera.v = bot->v;
    camera.isExternal = isExternal;
    camera.setTagged( null );
  }

  void BotProxy::update()
  {
    if( camera.bot == -1 ) {
      return;
    }

    Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

    if( ui::keyboard.keys[SDLK_TAB] && !ui::keyboard.oldKeys[SDLK_TAB] ) {
      ui::mouse.doShow = !ui::mouse.doShow;
    }
    if( ui::keyboard.keys[SDLK_i] && !ui::keyboard.oldKeys[SDLK_i] ) {
      bot->state &= ~Bot::PLAYER_BIT;
      camera.setBot( null );
      return;
    }

    /*
     * Camera
     */
    if( !isFreelook ) {
      bot->h = camera.h;
      bot->v = camera.v;
    }

    bot->actions = 0;

    /*
     * Movement
     */
    if( ui::keyboard.keys[SDLK_w] ) {
      bot->actions |= Bot::ACTION_FORWARD;
    }
    if( ui::keyboard.keys[SDLK_s] ) {
      bot->actions |= Bot::ACTION_BACKWARD;
    }
    if( ui::keyboard.keys[SDLK_d] ) {
      bot->actions |= Bot::ACTION_RIGHT;
    }
    if( ui::keyboard.keys[SDLK_a] ) {
      bot->actions |= Bot::ACTION_LEFT;
    }

    /*
     * Actions
     */
    if( ui::keyboard.keys[SDLK_SPACE] ) {
      bot->actions |= Bot::ACTION_JUMP;
    }
    if( ui::keyboard.keys[SDLK_LCTRL] ) {
      bot->actions |= Bot::ACTION_CROUCH;
    }
    if( ui::keyboard.keys[SDLK_LSHIFT] && !ui::keyboard.oldKeys[SDLK_LSHIFT] ) {
      bot->state ^= Bot::RUNNING_BIT;
    }
    if( ui::keyboard.keys[SDLK_x] ) {
      bot->actions |= Bot::ACTION_EXIT;
    }
    if( ui::keyboard.keys[SDLK_j] ) {
      bot->actions |= Bot::ACTION_EJECT;
    }
    if( ui::keyboard.keys[SDLK_LALT] && !ui::keyboard.oldKeys[SDLK_LALT] ) {
      isFreelook = !isFreelook;

      camera.h = bot->h;
      camera.v = bot->v;
    }
    if( ui::keyboard.keys[SDLK_p] && !ui::keyboard.oldKeys[SDLK_p] ) {
      bot->state ^= Bot::STEPPING_BIT;
    }

    bot->state &= ~( Bot::GESTURE0_BIT | Bot::GESTURE1_BIT | Bot::GESTURE2_BIT |
        Bot::GESTURE3_BIT | Bot::GESTURE4_BIT | Bot::GESTURE_ALL_BIT );

    if( ui::keyboard.keys[SDLK_f] ) {
      bot->state |= Bot::GESTURE0_BIT;
    }
    if( ui::keyboard.keys[SDLK_g] ) {
      bot->state |= Bot::GESTURE1_BIT;
    }
    if( ui::keyboard.keys[SDLK_h] ) {
      bot->state |= Bot::GESTURE2_BIT;
    }
    if( ui::keyboard.keys[SDLK_j] ) {
      bot->state |= Bot::GESTURE3_BIT;
    }
    if( ui::keyboard.keys[SDLK_k] ) {
      bot->state |= Bot::GESTURE4_BIT;
    }
    if( ui::keyboard.keys[SDLK_l] ) {
      bot->state |= Bot::GESTURE_ALL_BIT;
    }

    if( ui::keyboard.keys[SDLK_KP_ENTER] && !ui::keyboard.oldKeys[SDLK_KP_ENTER] ) {
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
        bot->actions |= Bot::ACTION_USE;
      }
      if( ui::mouse.wheelDown ) {
        bot->actions |= Bot::ACTION_TAKE;
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

    if( !isExternal ) {
      if( bot->parent != -1 ) { // inside vehicle
        hard_assert( orbis.objects[bot->parent]->flags & Object::VEHICLE_BIT );

        camera.w = 0.0f;
        camera.align();
        camera.warp( bot->p + camera.up * bot->camZ );

        bobPhi   = 0.0f;
        bobTheta = 0.0f;
        bobBias  = 0.0f;
      }
      else { // 1st person, not in vehicle
        const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

        if( bot->state & Bot::MOVING_BIT ) {
          if( bot->flags & Object::IN_WATER_BIT ) {
            float bobInc = ( bot->state & Bot::RUNNING_BIT ) && bot->grabObj == -1 ?
              clazz->bobSwimRunInc : clazz->bobSwimInc;

            bobPhi   = Math::mod( bobPhi + bobInc, Math::TAU );
            bobTheta = 0.0f;
            bobBias  = Math::sin( -2.0f * bobPhi ) * clazz->bobSwimAmplitude;
          }
          else if( ( bot->flags & Object::ON_FLOOR_BIT ) || bot->lower != -1 ) {
            float bobInc =
                ( bot->state & ( Bot::RUNNING_BIT | Bot::CROUCHING_BIT ) ) == Bot::RUNNING_BIT &&
                bot->grabObj == -1 ? clazz->bobRunInc : clazz->bobWalkInc;

            bobPhi   = Math::mod( bobPhi + bobInc, Math::TAU );
            bobTheta = Math::sin( bobPhi ) * clazz->bobRotation;
            bobBias  = Math::sin( 2.0f * bobPhi ) * clazz->bobAmplitude;
          }
          else {
            bobPhi   = 0.0f;
            bobTheta *= BOB_SUPPRESSION_COEF;
            bobBias  *= BOB_SUPPRESSION_COEF;
          }
        }
        else {
          bobPhi   = 0.0f;
          bobTheta *= BOB_SUPPRESSION_COEF;
          bobBias  *= BOB_SUPPRESSION_COEF;
        }
        if( bot->flags & Object::IN_WATER_BIT ) {
          bobTheta = 0.0f;
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
      if( bot->parent != -1 ) {
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
    else if( bot->grabObj != -1 ) {
      camera.setTagged( orbis.objects[camera.botObj->grabObj] );
    }
    else if( isExternal && isFreelook ) {
      // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
      float hvsc[6];

      Math::sincos( bot->h, &hvsc[0], &hvsc[1] );
      Math::sincos( bot->v, &hvsc[2], &hvsc[3] );

      hvsc[4] = hvsc[2] * hvsc[0];
      hvsc[5] = hvsc[2] * hvsc[1];

      // at vector must be based on bot's orientation, not on camera's
      Vec3 at = Vec3( -hvsc[4], hvsc[5], -hvsc[3] );

      float distance = static_cast<const BotClass*>( camera.botObj->clazz )->grabDistance;
      collider.mask = ~0;
      collider.translate( camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ ),
                          at * distance,
                          camera.botObj );
      collider.mask = Object::SOLID_BIT;

      camera.setTagged( collider.hit.obj );
    }
    else {
      float distance = static_cast<const BotClass*>( camera.botObj->clazz )->grabDistance;
      collider.mask = ~0;
      collider.translate( camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ ),
                          camera.at * distance,
                          camera.botObj );
      collider.mask = Object::SOLID_BIT;

      camera.setTagged( collider.hit.obj );
    }
  }

  void BotProxy::init()
  {
    externalDistFactor = config.getSet( "botProxy.externalDistFactor", 2.75f );
    isExternal         = config.getSet( "botProxy.isExternal", false );
    isFreelook         = false;
  }

}
}
