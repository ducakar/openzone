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
 * @file client/ui/HudArea.cc
 */

#include "stable.hh"

#include "client/ui/HudArea.hh"

#include "client/Shape.hh"
#include "client/Camera.hh"
#include "client/Context.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{
namespace ui
{

const float HudArea::VEHICLE_DIM      = VEHICLE_SIZE / 2.0f;
const float HudArea::CROSS_FADE_COEFF = 8.0f;

void HudArea::drawBotCrosshair()
{
  const Bot*      me      = camera.botObj;
  const BotClass* myClazz = static_cast<const BotClass*>( camera.botObj->clazz );

  float delta  = max( 1.0f - Math::fabs( camera.unit.headRot.w ), 0.0f );
  float alpha  = 1.0f - CROSS_FADE_COEFF * Math::sqrt( delta );
  float life   = max( 2.0f * me->life / myClazz->life - 1.0f, 0.0f );
  Vec4  colour = Math::mix( Vec4( 1.00f, 0.50f, 0.25f, alpha ),
                            Vec4( 1.00f, 1.00f, 1.00f, alpha ),
                            life );

  shape.colour( colour );
  glBindTexture( GL_TEXTURE_2D, crossTexId );
  shape.fill( crossIconX, crossIconY, ICON_SIZE, ICON_SIZE );
  glBindTexture( GL_TEXTURE_2D, 0 );

  if( me->parent < 0 && ( camera.object >= 0 || camera.entity >= 0 ) ) {
    const Object*      obj   = camera.objectObj;
    const ObjectClass* clazz = obj == null ? null : obj->clazz;
    const Dynamic*     dyn   = static_cast<const Dynamic*>( obj );
    const Bot*         bot   = static_cast<const Bot*>( obj );
    const Entity*      ent   = camera.entityObj;
    const Model*       model = ent == null ? null : ent->model;

    // it might happen that bot itself is tagged object for a frame when switching from freecam
    // into a bot
    if( obj == camera.botObj ) {
      return;
    }

    if( ent != null ) {
      if( lastEntityId != camera.entity ) {
        lastEntityId = camera.entity;

        title.set( descTextX, descTextY, ALIGN_CENTRE, Font::LARGE, "%s", model->title.cstr() );
      }

      title.draw( this, false );

      if( model->target >= 0 && ent->key >= 0 ) {
        glBindTexture( GL_TEXTURE_2D, useTexId );
        shape.fill( rightIconX, rightIconY, ICON_SIZE, ICON_SIZE );
      }

      if( ent->key < 0 ) {
        glBindTexture( GL_TEXTURE_2D, lockedTexId );
        shape.fill( bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE );
      }
      else if( ent->key > 0 ) {
        glBindTexture( GL_TEXTURE_2D, unlockedTexId );
        shape.fill( bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE );
      }
    }
    else {
      float life;
      if( obj->flags & Object::BOT_BIT ) {
        life = max( 2.0f * obj->life / clazz->life - 1.0f, 0.0f );
      }
      else {
        life = obj->life / clazz->life;
      }

      int lifeWidth = int( life * float( ICON_SIZE + 14 ) );

      shape.colour( 1.0f - life, life, 0.0f, 0.6f );
      shape.fill( healthBarX + 1, healthBarY + 11, lifeWidth, 10 );

      shape.colour( 0.0f, 0.0f, 0.0f, 0.15f );
      shape.fill( healthBarX + 1 + lifeWidth, healthBarY + 11, ICON_SIZE + 14 - lifeWidth, 10 );

      shape.colour( 1.0f, 1.0f, 1.0f, 0.8f );
      shape.rect( healthBarX, healthBarY + 10, ICON_SIZE + 16, 12 );

      if( lastObjectId != camera.object ) {
        lastObjectId = camera.object;

        String sTitle = ( obj->flags & Object::BOT_BIT ) && !bot->name.isEmpty() ?
                        bot->name + " (" + clazz->title + ")" : clazz->title;

        title.set( descTextX, descTextY, ALIGN_CENTRE, Font::LARGE, "%s", sTitle.cstr() );
      }

      title.draw( this, false );

      if( obj->flags & Object::BROWSABLE_BIT ) {
        glBindTexture( GL_TEXTURE_2D, browseTexId );
        shape.fill( leftIconX, leftIconY, ICON_SIZE, ICON_SIZE );
      }
      if( ( obj->flags & Object::USE_FUNC_BIT ) &&
          !( obj->flags & ( Object::WEAPON_BIT | Object::VEHICLE_BIT ) ) )
      {

        glBindTexture( GL_TEXTURE_2D, obj->flags & Object::USE_FUNC_BIT ? useTexId : deviceTexId );
        shape.fill( rightIconX, rightIconY, ICON_SIZE, ICON_SIZE );
      }

      if( !( obj->flags & Object::SOLID_BIT ) ) {
        glBindTexture( GL_TEXTURE_2D, 0 );
        return;
      }

      if( obj->flags & Object::VEHICLE_BIT ) {
        const Vehicle* vehicle = static_cast<const Vehicle*>( obj );

        if( vehicle->pilot < 0 ) {
          glBindTexture( GL_TEXTURE_2D, mountTexId );
          shape.fill( rightIconX, rightIconY, ICON_SIZE, ICON_SIZE );
        }
      }
      else if( obj->flags & Object::WEAPON_BIT ) {
        const WeaponClass* clazz = static_cast<const WeaponClass*>( obj->clazz );

        if( myClazz->name.beginsWith( clazz->userBase ) ) {
          glBindTexture( GL_TEXTURE_2D, equipTexId );
          shape.fill( rightIconX, rightIconY, ICON_SIZE, ICON_SIZE );
        }
      }

      if( obj->flags & Object::ITEM_BIT ) {
        glBindTexture( GL_TEXTURE_2D, takeTexId );
        shape.fill( leftIconX, leftIconY, ICON_SIZE, ICON_SIZE );
      }

      if( me->cargo < 0 && me->weapon < 0 &&
        ( obj->flags & Object::DYNAMIC_BIT ) && dyn->mass <= myClazz->grabMass &&
          // not swimming or on ladder
          !( me->state & ( Bot::SWIMMING_BIT | Bot::CLIMBING_BIT ) ) &&
          // if it is not a bot that is holding something
          ( !( obj->flags & Object::BOT_BIT ) || bot->cargo < 0 ) )
      {
        float dimX = bot->dim.x + dyn->dim.x;
        float dimY = bot->dim.y + dyn->dim.y;
        float dist = Math::sqrt( dimX*dimX + dimY*dimY ) + Bot::GRAB_EPSILON;

        if( dist <= myClazz->reachDist ) {
          glBindTexture( GL_TEXTURE_2D, liftTexId );
          shape.fill( bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE );
        }
      }
      if( camera.botObj->cargo >= 0 ) {
        glBindTexture( GL_TEXTURE_2D, grabTexId );
        shape.fill( bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE );
      }
    }
    glBindTexture( GL_TEXTURE_2D, 0 );
  }
}

void HudArea::drawBotStatus()
{
  const Bot*      bot      = camera.botObj;
  const BotClass* botClazz = static_cast<const BotClass*>( camera.botObj->clazz );

  float life         = 2.0f * bot->life / botClazz->life - 1.0f;
  float stamina      = bot->stamina / botClazz->stamina;
  int   lifeWidth    = max( int( life * 198.0f ), 0 );
  int   staminaWidth = max( int( stamina * 198.0f ), 0 );

  shape.colour( 1.0f - life, life, 0.0f, 0.6f );
  shape.fill( 9, 31, lifeWidth, 12 );
  shape.colour( 0.7f - 0.7f * stamina, 0.3f, 0.5f + 0.5f * stamina, 0.6f );
  shape.fill( 9, 9, staminaWidth, 12 );

  shape.colour( 0.0f, 0.0f, 0.0f, 0.15f );
  shape.fill( 9 + lifeWidth, 31, 198 - lifeWidth, 12 );
  shape.fill( 9 + staminaWidth, 9, 198 - staminaWidth, 12 );

  shape.colour( 1.0f, 1.0f, 1.0f, 0.6f );
  shape.rect( 8, 30, 200, 14 );
  shape.rect( 8, 8, 200, 14 );

  if( bot->weapon >= 0 && orbis.objects[bot->weapon] != null ) {
    const Weapon* weaponObj = static_cast<const Weapon*>( orbis.objects[bot->weapon] );

    shape.colour( 0.0f, 0.0f, 0.0f, 0.3f );
    shape.fill( 8, 52, 200, Font::INFOS[Font::LARGE].height + 8 );

    if( lastWeaponId != bot->weapon ) {
      lastWeaponId = bot->weapon;
      weaponName.set( "%s", weaponObj->clazz->title.cstr() );
    }
    if( lastWeaponRounds != weaponObj->nRounds ) {
      lastWeaponRounds = weaponObj->nRounds;

      if( weaponObj->nRounds < 0 ) {
        weaponRounds.set( "∞" );
      }
      else {
        weaponRounds.set( "%d", weaponObj->nRounds );
      }
    }

    weaponName.draw( this, false );
    weaponRounds.draw( this, true );
  }
}

void HudArea::drawVehicleStatus()
{
  const Bot*          bot      = camera.botObj;
  const Vehicle*      vehicle  = static_cast<const Vehicle*>( orbis.objects[bot->parent] );
  const VehicleClass* vehClazz = static_cast<const VehicleClass*>( vehicle->clazz );

  glEnable( GL_DEPTH_TEST );
  glDisable( GL_BLEND );

  float size = vehicle->dim.fastN();
  float scale = VEHICLE_DIM / size;
  int x = camera.width - 208 + VEHICLE_SIZE / 2;
  int y = 52 + vehClazz->nWeapons * ( Font::INFOS[Font::LARGE].height + 8 ) + VEHICLE_SIZE / 2;

  tf.camera = Mat44::translation( Vec3( float( x ), float( y ), 0.0f ) );
  tf.camera.scale( Vec3( 1.0f, 1.0f, 0.001f ) );

  tf.model = Mat44::scaling( Vec3( scale, scale, scale ) );
  tf.model.rotateX( Math::rad( -45.0f ) );
  tf.model.rotateZ( Math::rad( +160.0f ) );
  tf.applyCamera();

  context.drawImago( vehicle, null, Mesh::SOLID_BIT | Mesh::ALPHA_BIT );

  glEnable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );

  shape.bind();
  shader.program( shader.plain );

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, 0 );

  tf.camera = Mat44::ID;
  tf.applyCamera();

  float life      = vehicle->life / vehClazz->life;
  int   lifeWidth = max( int( life * 198.0f ), 0 );
  float fuel      = vehicle->fuel / vehClazz->fuel;
  int   fuelWidth = int( fuel * 198.0f );

  shape.colour( 1.0f - life, life, 0.0f, 0.6f );
  shape.fill( width - 207, 31, lifeWidth, 12 );
  shape.colour( 0.7f - 0.7f * fuel, 0.3f, 0.5f + 0.5f * fuel, 0.6f );
  shape.fill( width - 207, 9, fuelWidth, 12 );

  shape.colour( 0.0f, 0.0f, 0.0f, 0.15f );
  shape.fill( width - 207 + lifeWidth, 31, 198 - lifeWidth, 12 );
  shape.fill( width - 207 + fuelWidth, 9, 198 - fuelWidth, 12 );

  shape.colour( 1.0f, 1.0f, 1.0f, 0.6f );
  shape.rect( width - 208, 30, 200, 14 );
  shape.rect( width - 208, 8, 200, 14 );

  if( lastVehicleId != bot->parent ) {
    lastVehicleId = bot->parent;

    for( int i = 0; i < vehClazz->nWeapons; ++i ) {
      int labelIndex = vehClazz->nWeapons - i - 1;

      vehicleWeaponNames[labelIndex].set( "%s", vehClazz->weaponTitles[i].cstr() );
    }
  }

  for( int i = 0; i < vehClazz->nWeapons; ++i ) {
    if( i == vehicle->weapon ) {
      int step = font.INFOS[Font::LARGE].height + 8;

      shape.colour( 0.0f, 0.0f, 0.0f, 0.3f );
      shape.fill( width - 208, 52 + ( vehClazz->nWeapons - 1 - i ) * step,
                  200, Font::INFOS[Font::LARGE].height + 8 );
    }

    int labelIndex = vehClazz->nWeapons - i - 1;

    if( lastVehicleWeaponRounds[labelIndex] != vehicle->nRounds[i] ) {
      lastVehicleWeaponRounds[labelIndex] = vehicle->nRounds[i];

      if( vehicle->nRounds[i] < 0 ) {
        vehicleWeaponRounds[labelIndex].set( "∞" );
      }
      else {
        vehicleWeaponRounds[labelIndex].set( "%d", vehicle->nRounds[i] );
      }
    }

    vehicleWeaponNames[labelIndex].draw( this, false );
    vehicleWeaponRounds[labelIndex].draw( this, true );
  }
}

void HudArea::onReposition()
{
  width       = camera.width;
  height      = camera.height;

  crossIconX  = ( width - ICON_SIZE ) / 2;
  crossIconY  = ( height - ICON_SIZE ) / 2;
  leftIconX   = crossIconX - ICON_SIZE;
  leftIconY   = crossIconY;
  rightIconX  = crossIconX + ICON_SIZE;
  rightIconY  = crossIconY;
  bottomIconX = crossIconX;
  bottomIconY = crossIconY - ICON_SIZE;
  healthBarX  = crossIconX - 8;
  healthBarY  = crossIconY + ICON_SIZE;
  descTextX   = width / 2;
  descTextY   = crossIconY + ICON_SIZE + 36;
}

void HudArea::onUpdate()
{
  const Bot* bot = camera.botObj;

  // we need this is onUpdate() rather than in onDraw() for the rare case if an object is replaced
  // by a new one with the same id (onDraw() may not be called each frame and may miss this switch)
  if( camera.object != lastObjectId ) {
    lastObjectId = -1;
  }
  if( camera.entity != lastEntityId ) {
    lastEntityId = -1;
  }
  if( camera.state != Camera::UNIT || camera.bot < 0 ) {
    lastWeaponId = -1;
  }
  else if( bot->parent < 0 || orbis.objects[bot->parent] == null ) {
    lastVehicleId = -1;
  }
}

void HudArea::onDraw()
{
  if( camera.bot < 0 ) {
    show( false );
    return;
  }

  drawBotCrosshair();
  drawBotStatus();

  int parent = camera.botObj->parent;
  if( parent >= 0 && orbis.objects[parent] != null ) {
    drawVehicleStatus();
  }

  drawChildren();
}

HudArea::HudArea() :
  Area( camera.width, camera.height ),
  weaponName( 16, 54, ALIGN_LEFT, Font::LARGE, " " ),
  weaponRounds( 200, 54, ALIGN_RIGHT, Font::LARGE, "∞" ),
  lastObjectId( -1 ),
  lastEntityId( -1 ),
  lastWeaponId( -1 ),
  lastWeaponRounds( -1 ),
  lastVehicleId( -1 )
{
  flags = UPDATE_BIT | IGNORE_BIT | PINNED_BIT;

  int step = font.INFOS[Font::LARGE].height + 8;
  for( int i = 0; i < Vehicle::MAX_WEAPONS; ++i ) {
    lastVehicleWeaponRounds[i] = -1;

    vehicleWeaponNames[i].set( -200, 54 + i * step, ALIGN_LEFT, Font::LARGE, " " );
    vehicleWeaponRounds[i].set( -16, 54 + i * step, ALIGN_RIGHT, Font::LARGE, "∞" );
  }

  Log::verboseMode = true;

  crossTexId    = context.loadTextureLayer( "ui/icon/crosshair.ozIcon" );
  useTexId      = context.loadTextureLayer( "ui/icon/use.ozIcon" );
  deviceTexId   = context.loadTextureLayer( "ui/icon/device.ozIcon" );
  equipTexId    = context.loadTextureLayer( "ui/icon/equip.ozIcon" );
  mountTexId    = context.loadTextureLayer( "ui/icon/mount.ozIcon" );
  takeTexId     = context.loadTextureLayer( "ui/icon/take.ozIcon" );
  browseTexId   = context.loadTextureLayer( "ui/icon/browse.ozIcon" );
  liftTexId     = context.loadTextureLayer( "ui/icon/lift.ozIcon" );
  grabTexId     = context.loadTextureLayer( "ui/icon/grab.ozIcon" );
  lockedTexId   = context.loadTextureLayer( "ui/icon/locked.ozIcon" );
  unlockedTexId = context.loadTextureLayer( "ui/icon/unlocked.ozIcon" );

  Log::verboseMode = false;

  onReposition();

  title.set( descTextX, descTextY, ALIGN_CENTRE, Font::LARGE, " " );
}

HudArea::~HudArea()
{
  glDeleteTextures( 1, &crossTexId );
  glDeleteTextures( 1, &useTexId );
  glDeleteTextures( 1, &deviceTexId );
  glDeleteTextures( 1, &equipTexId );
  glDeleteTextures( 1, &mountTexId );
  glDeleteTextures( 1, &takeTexId );
  glDeleteTextures( 1, &browseTexId );
  glDeleteTextures( 1, &liftTexId );
  glDeleteTextures( 1, &grabTexId );
  glDeleteTextures( 1, &lockedTexId );
  glDeleteTextures( 1, &unlockedTexId );
}

}
}
}
