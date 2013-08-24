/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file matrix/luaapi.hh
 *
 * Matrix Lua API implementation.
 */

#pragma once

#include <common/luaapi.hh>
#include <matrix/Liber.hh>
#include <matrix/Vehicle.hh>
#include <matrix/Physics.hh>
#include <matrix/Synapse.hh>

namespace oz
{

enum AddMode
{
  ADD_FORCE = 0,
  ADD_TRY   = 1
};

enum CollideMode
{
  COLLIDE_STRUCTS_BIT     = 0x01,
  COLLIDE_OBJECTS_BIT     = 0x02,
  COLLIDE_ALL_OBJECTS_BIT = 0x04
};

struct MatrixLuaState
{
  Object*       self;
  Bot*          user;

  Struct*       str;
  Entity*       ent;
  Object*       obj;
  Frag*         frag;

  int           strIndex;
  int           objIndex;

  float         status;

  List<Struct*> structs;
  List<Object*> objects;
};

static MatrixLuaState ms;

/// @addtogroup luaapi
/// @{

/*
 * Orbis
 */

static int ozOrbisGetGravity( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( physics.gravity );
  return 1;
}

static int ozOrbisSetGravity( lua_State* l )
{
  ARG( 1 );

  physics.gravity = l_tofloat( 1 );
  return 0;
}

static int ozOrbisAddStr( lua_State* l )
{
  VARG( 5, 7 );

  const BSP* bsp = liber.bsp( l_tostring( 2 ) );

  AddMode mode    = AddMode( l_toint( 1 ) );
  Point   p       = Point( l_tofloat( 3 ), l_tofloat( 4 ), l_tofloat( 5 ) );
  Heading heading = Heading( Math::rand( 4 ) );
  bool    empty   = false;

  int nParams = l_gettop();
  for( int i = 6; i <= nParams; ++i ) {
    if( l_type( i ) == LUA_TBOOLEAN ) {
      empty = l_tobool( i );
    }
    else {
      heading = Heading( l_toint( i ) );
    }
  }

  if( mode != ADD_FORCE ) {
    Bounds bounds = rotate( *bsp, heading ) + ( p - Point::ORIGIN );

    if( collider.overlaps( bounds.toAABB() ) ) {
      ms.str = nullptr;
      l_pushint( -1 );
      return 1;
    }
  }

  ms.str = synapse.add( bsp, p, heading, empty );
  l_pushint( ms.str == nullptr ? -1 : ms.str->index );
  return 1;
}

static int ozOrbisAddObj( lua_State* l )
{
  VARG( 5, 7 );

  const ObjectClass* clazz = liber.objClass( l_tostring( 2 ) );

  AddMode mode    = AddMode( l_toint( 1 ) );
  Point   p       = Point( l_tofloat( 3 ), l_tofloat( 4 ), l_tofloat( 5 ) );
  Heading heading = Heading( Math::rand( 4 ) );
  bool    empty   = false;

  int nParams = l_gettop();
  for( int i = 6; i <= nParams; ++i ) {
    if( l_type( i ) == LUA_TBOOLEAN ) {
      empty = l_tobool( i );
    }
    else {
      heading = Heading( l_toint( i ) );
    }
  }

  if( mode != ADD_FORCE ) {
    Vec3 dim  = clazz->dim + Vec3( 2.0f*EPSILON, 2.0f*EPSILON, 2.0f*EPSILON );
    AABB aabb = AABB( p, rotate( dim, heading ) );

    if( collider.overlaps( aabb ) ) {
      ms.obj = nullptr;
      l_pushint( -1 );
      return 1;
    }
  }

  ms.obj = synapse.add( clazz, p, heading, empty );
  l_pushint( ms.obj == nullptr ? -1 : ms.obj->index );
  return 1;
}

static int ozOrbisAddFrag( lua_State* l )
{
  ARG( 8 );

  const FragPool* pool = liber.fragPool( l_tostring( 2 ) );

  AddMode mode     = AddMode( l_toint( 1 ) );
  Point   p        = Point( l_tofloat( 3 ), l_tofloat( 4 ), l_tofloat( 5 ) );
  Vec3    velocity = Vec3( l_tofloat( 6 ), l_tofloat( 7 ), l_tofloat( 8 ) );

  if( mode != ADD_FORCE ) {
    if( collider.overlaps( p ) ) {
      ms.frag = nullptr;
      l_pushint( -1 );
      return 1;
    }
  }

  ms.frag = synapse.add( pool, p, velocity );
  l_pushint( ms.frag == nullptr ? -1 : ms.frag->index );
  return 1;
}

static int ozOrbisGenFrags( lua_State* l )
{
  ARG( 11 );

  const FragPool* pool = liber.fragPool( l_tostring( 1 ) );

  int    nFrags   = l_toint( 2 );
  Bounds bb       = Bounds( Point( l_tofloat( 3 ), l_tofloat( 4 ), l_tofloat( 5 ) ),
                            Point( l_tofloat( 6 ), l_tofloat( 7 ), l_tofloat( 8 ) ) );
  Vec3   velocity = Vec3( l_tofloat( 9 ), l_tofloat( 10 ), l_tofloat( 11 ) );

  synapse.gen( pool, nFrags, bb, velocity );
  ms.frag = nullptr;
  return 0;
}

static int ozOrbisOverlaps( lua_State* l )
{
  VARG( 7, 8 );

  int  flags = l_toint( 1 );
  AABB aabb  = AABB( Point( l_tofloat( 2 ), l_tofloat( 3 ), l_tofloat( 4 ) ),
                     Vec3( l_tofloat( 5 ), l_tofloat( 6 ), l_tofloat( 7 ) ) );

  const Object* exclObj = nullptr;
  if( l_gettop() == 8 ) {
    int index = l_toint( 8 );

    if( uint( index ) >= uint( Orbis::MAX_OBJECTS ) ) {
      ERROR( "Invalid excluded object index (out of range)" );
    }

    exclObj = orbis.obj( index );
  }

  hard_assert( collider.mask == Object::SOLID_BIT );

  if( flags & COLLIDE_ALL_OBJECTS_BIT ) {
    collider.mask = ~0;
  }

  bool overlaps = collider.overlaps( aabb, exclObj );
  collider.mask = Object::SOLID_BIT;

  l_pushbool( overlaps );
  return 1;
}

static int ozOrbisBindOverlaps( lua_State* l )
{
  ARG( 7 );

  int  flags = l_toint( 1 );
  AABB aabb  = AABB( Point( l_tofloat( 2 ), l_tofloat( 3 ), l_tofloat( 4 ) ),
                     Vec3( l_tofloat( 5 ), l_tofloat( 6 ), l_tofloat( 7 ) ) );

  if( !( flags & ( COLLIDE_STRUCTS_BIT | COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) ) {
    ERROR( "At least one of OZ_STRUCTS_BIT, OZ_OBJECTS_BIT or OZ_ALL_OBJECTS_BIT must be given" );
  }

  List<Struct*>* structs = nullptr;
  List<Object*>* objects = nullptr;

  hard_assert( collider.mask == Object::SOLID_BIT );

  if( flags & COLLIDE_STRUCTS_BIT ) {
    structs = &ms.structs;

    ms.strIndex = 0;
    ms.structs.clear();
  }
  if( flags & ( COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) {
    objects = &ms.objects;

    ms.objIndex = 0;
    ms.objects.clear();
  }
  if( flags & COLLIDE_ALL_OBJECTS_BIT ) {
    collider.mask = ~0;
  }

  collider.getOverlaps( aabb, structs, objects );
  collider.mask = Object::SOLID_BIT;
  return 0;
}

/*
 * Caelum
 */

static int ozCaelumLoad( lua_State* l )
{
  ARG( 1 );

  const char* name = l_tostring( 1 );
  int id = String::isEmpty( name ) ? -1 : liber.caelumIndex( name );

  orbis.caelum.id = id;
  return 0;
}

static int ozCaelumGetHeading( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( orbis.caelum.heading );
  return 1;
}

static int ozCaelumSetHeading( lua_State* l )
{
  ARG( 1 );

  orbis.caelum.heading = l_tofloat( 1 );
  return 0;
}

static int ozCaelumGetPeriod( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( orbis.caelum.period );
  return 1;
}

static int ozCaelumSetPeriod( lua_State* l )
{
  ARG( 1 );

  orbis.caelum.period = l_tofloat( 1 );
  return 0;
}

static int ozCaelumGetTime( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( orbis.caelum.time );
  return 1;
}

static int ozCaelumSetTime( lua_State* l )
{
  ARG( 1 );

  orbis.caelum.time = l_tofloat( 1 );
  return 0;
}

static int ozCaelumAddTime( lua_State* l )
{
  ARG( 1 );

  orbis.caelum.time += l_tofloat( 1 );
  return 0;
}

static int ozCaelumSetRealTime( lua_State* l )
{
  ARG( 0 );

  Time  localTime = Time::local();
  // Since it cannot be determined whether DST is on, just subtract half an hour to get an average.
  int daySecs = localTime.hour * 60*60 + localTime.minute * 60 + localTime.second - 30*60;

  // caelum.time = 0 means 6:00 (beginning of a Roman day), so shift for one quarter of a day.
  orbis.caelum.time = Math::fmod( float( daySecs ) / 86400.0f + 0.75f, 1.0f ) * orbis.caelum.period;
  return 0;
}

/*
 * Terra
 */

static int ozTerraLoad( lua_State* l )
{
  ARG( 1 );

  const char* name = l_tostring( 1 );
  int id = String::isEmpty( name ) ? -1 : liber.terraIndex( name );

  orbis.terra.load( id );
  return 0;
}

static int ozTerraHeight( lua_State* l )
{
  ARG( 2 );

  float x = l_tofloat( 1 );
  float y = l_tofloat( 2 );

  l_pushfloat( orbis.terra.height( x, y ) );
  return 1;
}

/*
 * Structure
 */

static int ozBindStr( lua_State* l )
{
  ARG( 1 );

  int index = l_toint( 1 );
  if( uint( index ) >= uint( Orbis::MAX_STRUCTS ) ) {
    ms.str = nullptr;
  }
  else {
    ms.str = orbis.str( index );
  }

  l_pushbool( ms.str != nullptr );
  return 1;
}

static int ozBindNextStr( lua_State* l )
{
  ARG( 0 );

  ms.str = nullptr;

  while( ms.str == nullptr && ms.strIndex < ms.structs.length() ) {
    ms.str = ms.structs[ms.strIndex];
    ++ms.strIndex;
  }

  l_pushbool( ms.str != nullptr );
  return 1;
}

static int ozStrIsNull( lua_State* l )
{
  ARG( 0 );

  l_pushbool( ms.str == nullptr );
  return 1;
}

static int ozStrGetIndex( lua_State* l )
{
  ARG( 0 );

  l_pushint( ms.str == nullptr ? -1 : ms.str->index );
  return 1;
}

static int ozStrGetBounds( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushfloat( ms.str->mins.x );
  l_pushfloat( ms.str->mins.y );
  l_pushfloat( ms.str->mins.z );
  l_pushfloat( ms.str->maxs.x );
  l_pushfloat( ms.str->maxs.y );
  l_pushfloat( ms.str->maxs.z );
  return 6;
}

static int ozStrGetPos( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushfloat( ms.str->p.x );
  l_pushfloat( ms.str->p.y );
  l_pushfloat( ms.str->p.z );
  return 3;
}

static int ozStrGetBSP( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushstring( ms.str->bsp->name );
  return 1;
}

static int ozStrGetHeading( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushint( ms.str->heading );
  return 1;
}

static int ozStrMaxLife( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushfloat( ms.str->bsp->life );
  return 1;
}

static int ozStrGetLife( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushfloat( ms.str->life );
  return 1;
}

static int ozStrSetLife( lua_State* l )
{
  ARG( 1 );
  STR();

  ms.str->life = clamp( l_tofloat( 1 ), 0.0f, ms.str->bsp->life );
  return 0;
}

static int ozStrAddLife( lua_State* l )
{
  ARG( 1 );
  STR();

  ms.str->life = clamp( ms.str->life + l_tofloat( 1 ), 0.0f, ms.str->bsp->life );
  return 0;
}

static int ozStrDefaultResistance( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushfloat( ms.str->bsp->resistance );
  return 1;
}

static int ozStrGetResistance( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushfloat( ms.str->resistance );
  return 1;
}

static int ozStrSetResistance( lua_State* l )
{
  ARG( 1 );
  STR();

  ms.str->resistance = l_tofloat( 1 );
  return 0;
}

static int ozStrDamage( lua_State* l )
{
  ARG( 1 );
  STR();

  ms.str->damage( l_tofloat( 1 ) );
  return 0;
}

static int ozStrDestroy( lua_State* l )
{
  ARG( 0 );
  STR();

  ms.str->destroy();
  return 0;
}

static int ozStrRemove( lua_State* l )
{
  ARG( 0 );
  STR();

  synapse.remove( ms.str );
  ms.str = nullptr;
  return 0;
}

static int ozStrNumBoundObjs( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushint( ms.str->boundObjects.length() );
  return 1;
}

static int ozStrBindBoundObj( lua_State* l )
{
  ARG( 1 );
  STR();

  int index = l_toint( 1 );
  if( uint( index ) >= uint( ms.str->boundObjects.length() ) ) {
    ERROR( "Invalid structure bound object index (out of range)" );
  }

  ms.obj = orbis.obj( ms.str->boundObjects[index] );
  l_pushbool( ms.obj != nullptr );
  return 1;
}

static int ozStrNumEnts( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushint( ms.str->entities.length() );
  return 1;
}

static int ozStrBindEnt( lua_State* l )
{
  ARG( 1 );
  STR();
  ENT_INDEX( l_toint( 1 ) );

  ms.ent = ent;
  l_pushbool( ms.ent != nullptr );
  return 1;
}

static int ozStrOverlaps( lua_State* l )
{
  ARG( 2 );
  STR();

  int  flags = l_toint( 1 );
  AABB aabb  = ms.str->toAABB( l_tofloat( 2 ) );

  hard_assert( collider.mask == Object::SOLID_BIT );

  if( flags & COLLIDE_ALL_OBJECTS_BIT ) {
    collider.mask = ~0;
  }

  bool overlaps = collider.overlaps( aabb );
  collider.mask = Object::SOLID_BIT;

  l_pushbool( overlaps );
  return 1;
}

static int ozStrBindOverlaps( lua_State* l )
{
  ARG( 2 );
  STR();

  int  flags = l_toint( 1 );
  AABB aabb  = ms.str->toAABB( l_tofloat( 2 ) );

  if( !( flags & ( COLLIDE_STRUCTS_BIT | COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) ) {
    ERROR( "At least one of OZ_STRUCTS_BIT, OZ_OBJECTS_BIT or OZ_ALL_OBJECTS_BIT must be given" );
  }

  List<Struct*>* structs = nullptr;
  List<Object*>* objects = nullptr;

  hard_assert( collider.mask == Object::SOLID_BIT );

  if( flags & COLLIDE_STRUCTS_BIT ) {
    structs = &ms.structs;

    ms.strIndex = 0;
    ms.structs.clear();
  }
  if( flags & ( COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) {
    objects = &ms.objects;

    ms.objIndex = 0;
    ms.objects.clear();
  }
  if( flags & COLLIDE_ALL_OBJECTS_BIT ) {
    collider.mask = ~0;
  }

  collider.getOverlaps( aabb, structs, objects );
  collider.mask = Object::SOLID_BIT;
  return 0;
}

static int ozStrVectorFromSelf( lua_State* l )
{
  ARG( 1 );
  STR();
  SELF();

  Vec3 vec = ms.str->p - ms.self->p;

  l_pushfloat( vec.x );
  l_pushfloat( vec.y );
  l_pushfloat( vec.z );
  return 3;
}

static int ozStrVectorFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  vec = ms.str->p - eye;

  l_pushfloat( vec.x );
  l_pushfloat( vec.y );
  l_pushfloat( vec.z );
  return 3;
}

static int ozStrDirFromSelf( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF();

  Vec3 dir = ~( ms.str->p - ms.self->p );

  l_pushfloat( dir.x );
  l_pushfloat( dir.y );
  l_pushfloat( dir.z );
  return 3;
}

static int ozStrDirFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  dir = ~( ms.str->p - eye );

  l_pushfloat( dir.x );
  l_pushfloat( dir.y );
  l_pushfloat( dir.z );
  return 3;
}

static int ozStrDistFromSelf( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF();

  l_pushfloat( !( ms.str->p - ms.self->p ) );
  return 1;
}

static int ozStrDistFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );

  l_pushfloat( !( ms.str->p - eye ) );
  return 1;
}

static int ozStrHeadingFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF_BOT();

  float dx    = ms.str->p.x - self->p.x;
  float dy    = ms.str->p.y - self->p.y;
  float angle = Math::deg( angleWrap( Math::atan2( -dx, dy ) ) );

  l_pushfloat( angle );
  return 1;
}

static int ozStrRelHeadingFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF_BOT();

  float dx    = ms.str->p.x - self->p.x;
  float dy    = ms.str->p.y - self->p.y;
  float angle = Math::deg( angleDiff( Math::atan2( -dx, dy ), self->h ) );

  l_pushfloat( angle );
  return 1;
}

static int ozStrPitchFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF_BOT();

  Point eye   = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  float dx    = ms.str->p.x - eye.x;
  float dy    = ms.str->p.y - eye.y;
  float dz    = ms.str->p.z - eye.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  l_pushfloat( angle );
  return 1;
}

static int ozStrIsVisibleFromSelf( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF();

  Vec3 vec = ms.str->p - ms.self->p;

  collider.translate( ms.self->p, vec, ms.self );

  l_pushbool( collider.hit.str == ms.str || collider.hit.ratio == 1.0f );
  return 1;
}

static int ozStrIsVisibleFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  vec = ms.str->p - eye;

  collider.translate( eye, vec, self );

  l_pushbool( collider.hit.str == ms.str || collider.hit.ratio == 1.0f );
  return 1;
}

/*
 * Entity
 */

static int ozEntGetState( lua_State* l )
{
  ARG( 0 );
  ENT();

  l_pushint( ms.ent->state );
  return 1;
}

static int ozEntSetState( lua_State* l )
{
  ARG( 1 );
  ENT();

  Entity::State state = Entity::State( l_toint( 1 ) );

  ms.ent->time     = 0.0f;
  ms.ent->velocity = Vec3::ZERO;

  if( state == Entity::CLOSED ) {
    ms.ent->offset = Vec3::ZERO;
    ms.ent->state  = Entity::CLOSED;
    ms.ent->ratio  = 0.0f;
  }
  else if( state == Entity::OPENED ) {
    ms.ent->offset = ms.ent->clazz->move;
    ms.ent->state  = Entity::OPENED;
    ms.ent->ratio  = 1.0f;
  }
  else {
    ERROR( "Entity state should be either OZ_ENTITY_CLOSED or OZ_ENTITY_OPENED" );
  }
  return 0;
}

static int ozEntGetLock( lua_State* l )
{
  ARG( 0 );
  ENT();

  l_pushint( ms.ent->key );
  return 1;
}

static int ozEntSetLock( lua_State* l )
{
  ARG( 1 );
  ENT();

  ms.ent->key = l_toint( 1 );
  return 0;
}

static int ozEntTrigger( lua_State* l )
{
  ARG( 0 );
  ENT();

  ms.ent->trigger();
  return 0;
}

static int ozEntOverlaps( lua_State* l )
{
  ARG( 2 );
  ENT();

  int   flags  = l_toint( 1 );
  float margin = l_tofloat( 2 );

  hard_assert( collider.mask == Object::SOLID_BIT );

  if( flags & COLLIDE_ALL_OBJECTS_BIT ) {
    collider.mask = ~0;
  }

  bool overlaps = collider.overlaps( ms.ent, margin );
  collider.mask = Object::SOLID_BIT;

  l_pushbool( overlaps );
  return 1;
}

static int ozEntBindOverlaps( lua_State* l )
{
  ARG( 2 );
  ENT();

  int   flags  = l_toint( 1 );
  float margin = l_tofloat( 2 );

  if( !( flags & ( COLLIDE_STRUCTS_BIT | COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) ) {
    ERROR( "At least one of OZ_STRUCTS_BIT, OZ_OBJECTS_BIT or OZ_ALL_OBJECTS_BIT must be given" );
  }

  List<Object*>* objects = nullptr;

  hard_assert( collider.mask == Object::SOLID_BIT );

  if( flags & ( COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) {
    objects = &ms.objects;

    ms.objIndex = 0;
    ms.objects.clear();
  }
  if( flags & COLLIDE_ALL_OBJECTS_BIT ) {
    collider.mask = ~0;
  }

  collider.getOverlaps( ms.ent, objects, margin );
  collider.mask = Object::SOLID_BIT;
  return 0;
}

static int ozEntVectorFromSelf( lua_State* l )
{
  ARG( 1 );
  STR();
  ENT();
  SELF();

  Point p   = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );
  Vec3  vec = p - ms.self->p;

  l_pushfloat( vec.x );
  l_pushfloat( vec.y );
  l_pushfloat( vec.z );
  return 3;
}

static int ozEntVectorFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  ENT();
  SELF_BOT();

  Point p   = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );
  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  vec = p - eye;

  l_pushfloat( vec.x );
  l_pushfloat( vec.y );
  l_pushfloat( vec.z );
  return 3;
}

static int ozEntDirFromSelf( lua_State* l )
{
  ARG( 0 );
  STR();
  ENT();
  SELF();

  Point p   = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );
  Vec3  dir = ~( p - ms.self->p );

  l_pushfloat( dir.x );
  l_pushfloat( dir.y );
  l_pushfloat( dir.z );
  return 3;
}

static int ozEntDirFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  ENT();
  SELF_BOT();

  Point p   = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );
  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  dir = ~( p - eye );

  l_pushfloat( dir.x );
  l_pushfloat( dir.y );
  l_pushfloat( dir.z );
  return 3;
}

static int ozEntDistFromSelf( lua_State* l )
{
  ARG( 0 );
  STR();
  ENT();
  SELF();

  Point p = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );

  l_pushfloat( !( p - ms.self->p ) );
  return 1;
}

static int ozEntDistFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  ENT();
  SELF_BOT();

  Point p   = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );
  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );

  l_pushfloat( !( p - eye ) );
  return 1;
}

static int ozEntHeadingFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  ENT();
  SELF_BOT();

  Point p     = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );
  float dx    = p.x - self->p.x;
  float dy    = p.y - self->p.y;
  float angle = Math::deg( angleWrap( Math::atan2( -dx, dy ) ) );

  l_pushfloat( angle );
  return 1;
}

static int ozEntRelHeadingFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  ENT();
  SELF_BOT();

  Point p     = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );
  float dx    = p.x - self->p.x;
  float dy    = p.y - self->p.y;
  float angle = Math::deg( angleDiff( Math::atan2( -dx, dy ), self->h ) );

  l_pushfloat( angle );
  return 1;
}

static int ozEntPitchFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  ENT();
  SELF_BOT();

  Point p     = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );
  Point eye   = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  float dx    = p.x - eye.x;
  float dy    = p.y - eye.y;
  float dz    = p.z - eye.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  l_pushfloat( angle );
  return 1;
}

static int ozEntIsVisibleFromSelf( lua_State* l )
{
  ARG( 0 );
  STR();
  ENT();
  SELF();

  Point p   = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );
  Vec3  vec = p - ms.self->p;

  collider.translate( ms.self->p, vec, ms.self );

  l_pushbool( collider.hit.entity == ms.ent || collider.hit.ratio == 1.0f );
  return 1;
}

static int ozEntIsVisibleFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  ENT();
  SELF_BOT();

  Point p   = ms.str->toAbsoluteCS( ms.ent->clazz->p() + ms.ent->offset );
  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  vec = p - eye;

  collider.translate( eye, vec, self );

  l_pushbool( collider.hit.entity == ms.ent || collider.hit.ratio == 1.0f );
  return 1;
}

/*
 * Object
 */

static int ozBindObj( lua_State* l )
{
  ARG( 1 );

  int index = l_toint( 1 );
  if( uint( index ) >= uint( Orbis::MAX_OBJECTS ) ) {
    ms.obj = nullptr;
  }
  else {
    ms.obj = orbis.obj( index );
  }

  l_pushbool( ms.obj != nullptr );
  return 1;
}

static int ozBindSelf( lua_State* l )
{
  ARG( 0 );

  ms.obj = ms.self;

  l_pushbool( ms.obj != nullptr );
  return 1;
}

static int ozBindUser( lua_State* l )
{
  ARG( 0 );

  ms.obj = ms.user;

  l_pushbool( ms.obj != nullptr );
  return 1;
}

static int ozBindNextObj( lua_State* l )
{
  ARG( 0 );

  ms.obj = nullptr;

  while( ms.obj == nullptr && ms.objIndex < ms.objects.length() ) {
    ms.obj = ms.objects[ms.objIndex];
    ++ms.objIndex;
  }

  l_pushbool( ms.obj != nullptr );
  return 1;
}

static int ozObjIsNull( lua_State* l )
{
  ARG( 0 );

  l_pushbool( ms.obj == nullptr );
  return 1;
}

static int ozObjIsSelf( lua_State* l )
{
  ARG( 0 );

  l_pushbool( ms.obj == ms.self );
  return 1;
}

static int ozObjIsUser( lua_State* l )
{
  ARG( 0 );

  l_pushbool( ms.obj == ms.user );
  return 1;
}

static int ozObjIsCut( lua_State* l )
{
  ARG( 0 );

  l_pushbool( ms.obj != nullptr && ms.obj->cell == nullptr );
  return 1;
}

static int ozObjGetIndex( lua_State* l )
{
  ARG( 0 );

  l_pushint( ms.obj == nullptr ? -1 : ms.obj->index );
  return 1;
}

static int ozObjGetPos( lua_State* l )
{
  ARG( 0 );
  OBJ();

  if( ms.obj->cell == nullptr ) {
    hard_assert( ms.obj->flags & Object::DYNAMIC_BIT );

    const Dynamic* dyn    = static_cast<const Dynamic*>( ms.obj );
    const Object*  parent = orbis.obj( dyn->parent );

    if( parent != nullptr ) {
      l_pushfloat( parent->p.x );
      l_pushfloat( parent->p.y );
      l_pushfloat( parent->p.z );
      return 3;
    }
  }

  l_pushfloat( ms.obj->p.x );
  l_pushfloat( ms.obj->p.y );
  l_pushfloat( ms.obj->p.z );
  return 3;
}

static int ozObjWarpPos( lua_State* l )
{
  ARG( 3 );
  OBJ();

  ms.obj->p.x = l_tofloat( 1 );
  ms.obj->p.y = l_tofloat( 2 );
  ms.obj->p.z = l_tofloat( 3 );

  ms.obj->flags &= ~Object::MOVE_CLEAR_MASK;
  return 0;
}

static int ozObjGetDim( lua_State* l )
{
  ARG( 0 );
  OBJ();

  l_pushfloat( ms.obj->dim.x );
  l_pushfloat( ms.obj->dim.y );
  l_pushfloat( ms.obj->dim.z );
  return 3;
}

static int ozObjHasFlag( lua_State* l )
{
  ARG( 1 );

  l_pushbool( ms.obj != nullptr && ( ms.obj->flags & l_toint( 1 ) ) );
  return 1;
}

static int ozObjGetHeading( lua_State* l )
{
  ARG( 0 );
  OBJ();

  l_pushint( ms.obj->flags & Object::HEADING_MASK );
  return 1;
}

static int ozObjGetClassName( lua_State* l )
{
  ARG( 0 );
  OBJ();

  l_pushstring( ms.obj->clazz->name );
  return 1;
}

static int ozObjMaxLife( lua_State* l )
{
  ARG( 0 );
  OBJ();

  l_pushfloat( ms.obj->clazz->life );
  return 1;
}

static int ozObjGetLife( lua_State* l )
{
  ARG( 0 );
  OBJ();

  l_pushfloat( ms.obj->life );
  return 1;
}

static int ozObjSetLife( lua_State* l )
{
  ARG( 1 );
  OBJ();

  ms.obj->life = clamp( l_tofloat( 1 ), 0.0f, ms.obj->clazz->life );
  return 0;
}

static int ozObjAddLife( lua_State* l )
{
  ARG( 1 );
  OBJ();

  ms.obj->life = clamp( ms.obj->life + l_tofloat( 1 ), 0.0f, ms.obj->clazz->life );
  return 0;
}

static int ozObjDefaultResistance( lua_State* l )
{
  ARG( 0 );
  OBJ();

  l_pushfloat( ms.obj->clazz->resistance );
  return 1;
}

static int ozObjGetResistance( lua_State* l )
{
  ARG( 0 );
  OBJ();

  l_pushfloat( ms.obj->resistance );
  return 1;
}

static int ozObjSetResistance( lua_State* l )
{
  ARG( 1 );
  OBJ();

  ms.obj->resistance = l_tofloat( 1 );
  return 0;
}

static int ozObjAddEvent( lua_State* l )
{
  ARG( 2 );
  OBJ();

  int   id        = l_toint( 1 );
  float intensity = l_tofloat( 2 );

  if( id >= 0 && intensity < 0.0f ) {
    ERROR( "Event intensity for sounds (eventId >= 0) must be be >= 0.0" );
  }

  ms.obj->addEvent( id, intensity );
  return 0;
}

static int ozObjEnableUpdate( lua_State* l )
{
  ARG( 1 );
  OBJ();

  if( l_tobool( 1 ) ) {
    ms.obj->flags |= Object::UPDATE_FUNC_BIT;
  }
  else {
    ms.obj->flags &= ~Object::UPDATE_FUNC_BIT;
  }
  return 0;
}

static int ozObjReportStatus( lua_State* l )
{
  ARG( 1 );
  OBJ();

  ms.status = l_tofloat( 1 );
  return 0;
}

/**
 * Inflict damage to the object.
 *
 * @code void ozObjDamage( float damage ) @endcode
 *
 * If `damage` is greater than object's resistance, object receives that difference of damage and
 * `EVENT_DAMAGE` is generated.
 */
static int ozObjDamage( lua_State* l )
{
  ARG( 1 );
  OBJ();

  ms.obj->damage( l_tofloat( 1 ) );
  return 0;
}

/**
 * Destroy object.
 *
 * @code void ozObjDestroy( bool quiet = false ) @endcode
 *
 * %Object will be removed on the beginning of the next update. If `quiet` is true, `onDestroy`
 * handler call and `EVENT_DESTROY` will be suppressed.
 */
static int ozObjDestroy( lua_State* l )
{
  VARG( 0, 1 );
  OBJ();

  ms.obj->life = 0.0f;

  if( l_tobool( 1 ) ) {
    ms.obj->flags |= Object::DESTROYED_BIT;
  }
  return 0;
}

static int ozObjBindItems( lua_State* l )
{
  ARG( 0 );
  OBJ();

  ms.objIndex = 0;
  ms.objects.clear();

  foreach( item, ms.obj->items.citer() ) {
    hard_assert( *item >= 0 );

    ms.objects.add( orbis.obj( *item ) );
  }
  return 0;
}

static int ozObjBindItem( lua_State* l )
{
  ARG( 1 );
  OBJ();

  int index = l_toint( 1 );

  if( uint( index ) >= uint( ms.obj->items.length() ) ) {
    ERROR( "Invalid inventory item index" );
  }

  ms.obj = orbis.obj( ms.obj->items[index] );
  l_pushbool( ms.obj != nullptr );
  return 1;
}

static int ozObjAddItem( lua_State* l )
{
  ARG( 1 );
  OBJ();

  if( ms.obj->items.length() == ms.obj->clazz->nItems ) {
    l_pushbool( false );
    return 1;
  }

  Dynamic* newItem;

  if( l_type( 1 ) == LUA_TNUMBER ) {
    ITEM_INDEX( l_toint( 1 ) );

    if( item->cell == nullptr ) {
      hard_assert( item->parent >= 0 );

      Object* container = orbis.obj( item->parent );
      if( container != nullptr ) {
        container->items.exclude( item->index );
      }
    }

    newItem = item;
  }
  else {
    const char* sClazz = l_tostring( 1 );

    Object* obj = synapse.addObject( sClazz, Point::ORIGIN, Heading( Math::rand( 4 ) ), false );
    if( obj == nullptr ) {
      l_pushbool( false );
      return 1;
    }

    if( !( obj->flags & Object::ITEM_BIT ) ) {
      ERROR( "Tried to add non-item object to inventory" );
    }

    newItem = static_cast<Dynamic*>( obj );
  }

  newItem->parent = ms.obj->index;
  ms.obj->items.add( newItem->index );

  if( newItem->cell != nullptr ) {
    synapse.cut( newItem );
  }

  l_pushbool( true );
  return 1;
}

static int ozObjRemoveItem( lua_State* l )
{
  ARG( 1 );
  OBJ();

  int item = l_toint( 1 );
  if( uint( item ) >= uint( ms.obj->items.length() ) ) {
    ERROR( "Invalid item number (out of range)" );
  }

  synapse.removeObject( ms.obj->items[item] );
  return 0;
}

static int ozObjRemoveAllItems( lua_State* l )
{
  ARG( 0 );
  OBJ();

  foreach( item, ms.obj->items.citer() ) {
    synapse.removeObject( *item );
  }
  ms.obj->items.clear();
  return 0;
}

static int ozObjOverlaps( lua_State* l )
{
  ARG( 2 );
  OBJ();

  int  flags = l_toint( 1 );
  AABB aabb  = AABB( *ms.obj, l_tofloat( 2 ) );

  hard_assert( collider.mask == Object::SOLID_BIT );

  if( flags & COLLIDE_ALL_OBJECTS_BIT ) {
    collider.mask = ~0;
  }

  bool overlaps = collider.overlaps( aabb, ms.obj );
  collider.mask = Object::SOLID_BIT;

  l_pushbool( overlaps );
  return 1;
}

static int ozObjBindOverlaps( lua_State* l )
{
  ARG( 2 );
  OBJ();

  int  flags = l_toint( 1 );
  AABB aabb  = AABB( *ms.obj, l_tofloat( 2 ) );

  if( !( flags & ( COLLIDE_STRUCTS_BIT | COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) ) {
    ERROR( "At least one of OZ_STRUCTS_BIT, OZ_OBJECTS_BIT or OZ_ALL_OBJECTS_BIT must be given" );
  }

  List<Struct*>* structs = nullptr;
  List<Object*>* objects = nullptr;

  hard_assert( collider.mask == Object::SOLID_BIT );

  if( flags & COLLIDE_STRUCTS_BIT ) {
    structs = &ms.structs;

    ms.strIndex = 0;
    ms.structs.clear();
  }
  if( flags & ( COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) {
    objects = &ms.objects;

    ms.objIndex = 0;
    ms.objects.clear();
  }
  if( flags & COLLIDE_ALL_OBJECTS_BIT ) {
    collider.mask = ~0;
  }

  collider.getOverlaps( aabb, structs, objects );
  collider.mask = Object::SOLID_BIT;
  return 0;
}

static int ozObjVectorFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF();

  Vec3 vec = ms.obj->p - ms.self->p;

  l_pushfloat( vec.x );
  l_pushfloat( vec.y );
  l_pushfloat( vec.z );
  return 3;
}

static int ozObjVectorFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  vec = ms.obj->p - eye;

  l_pushfloat( vec.x );
  l_pushfloat( vec.y );
  l_pushfloat( vec.z );
  return 3;
}

static int ozObjDirFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF();

  Vec3 dir = ~( ms.obj->p - ms.self->p );

  l_pushfloat( dir.x );
  l_pushfloat( dir.y );
  l_pushfloat( dir.z );
  return 3;
}

static int ozObjDirFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  dir = ~( ms.obj->p - eye );

  l_pushfloat( dir.x );
  l_pushfloat( dir.y );
  l_pushfloat( dir.z );
  return 3;
}

static int ozObjDistFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF();

  l_pushfloat( !( ms.obj->p - ms.self->p ) );
  return 1;
}

static int ozObjDistFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );

  l_pushfloat( !( ms.obj->p - eye ) );
  return 1;
}

static int ozObjHeadingFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  float dx    = ms.obj->p.x - self->p.x;
  float dy    = ms.obj->p.y - self->p.y;
  float angle = Math::deg( angleWrap( Math::atan2( -dx, dy ) ) );

  l_pushfloat( angle );
  return 1;
}

static int ozObjRelHeadingFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  float dx    = ms.obj->p.x - self->p.x;
  float dy    = ms.obj->p.y - self->p.y;
  float angle = Math::deg( angleDiff( Math::atan2( -dx, dy ), self->h ) );

  l_pushfloat( angle );
  return 1;
}

static int ozObjPitchFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  Point eye   = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  float dx    = ms.obj->p.x - eye.x;
  float dy    = ms.obj->p.y - eye.y;
  float dz    = ms.obj->p.z - eye.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  l_pushfloat( angle );
  return 1;
}

static int ozObjIsVisibleFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF();

  Vec3 vec = ms.obj->p - ms.self->p;

  collider.translate( ms.self->p, vec, ms.self );

  l_pushbool( collider.hit.obj == ms.obj || collider.hit.ratio == 1.0f );
  return 1;
}

static int ozObjIsVisibleFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  vec = ms.obj->p - eye;

  collider.translate( eye, vec, self );

  l_pushbool( collider.hit.obj == ms.obj || collider.hit.ratio == 1.0f );
  return 1;
}

/*
 * Dynamic object
 */

static int ozDynGetParent( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_DYNAMIC();

  const Object* parent = orbis.obj( dyn->parent );

  l_pushint( parent == nullptr ? -1 : dyn->parent );
  return 1;
}

static int ozDynGetVelocity( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_DYNAMIC();

  l_pushfloat( dyn->velocity.x );
  l_pushfloat( dyn->velocity.y );
  l_pushfloat( dyn->velocity.z );
  return 3;
}

static int ozDynGetMomentum( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_DYNAMIC();

  l_pushfloat( dyn->momentum.x );
  l_pushfloat( dyn->momentum.y );
  l_pushfloat( dyn->momentum.z );
  return 3;
}

static int ozDynSetMomentum( lua_State* l )
{
  ARG( 3 );
  OBJ();
  OBJ_DYNAMIC();

  dyn->flags     &= ~Object::DISABLED_BIT;
  dyn->momentum.x = l_tofloat( 1 );
  dyn->momentum.y = l_tofloat( 2 );
  dyn->momentum.z = l_tofloat( 3 );
  return 0;
}

static int ozDynAddMomentum( lua_State* l )
{
  ARG( 3 );
  OBJ();
  OBJ_DYNAMIC();

  dyn->flags      &= ~Object::DISABLED_BIT;
  dyn->momentum.x += l_tofloat( 1 );
  dyn->momentum.y += l_tofloat( 2 );
  dyn->momentum.z += l_tofloat( 3 );
  return 0;
}

static int ozDynGetMass( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_DYNAMIC();

  l_pushfloat( dyn->mass );
  return 1;
}

static int ozDynGetLift( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_DYNAMIC();

  l_pushfloat( dyn->lift );
  return 1;
}

/*
 * Weapon
 */

static int ozWeaponMaxRounds( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_WEAPON();

  const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( weapon->clazz );

  l_pushint( weaponClazz->nRounds );
  return 1;
}

static int ozWeaponGetRounds( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_WEAPON();

  l_pushint( weapon->nRounds );
  return 1;
}

static int ozWeaponSetRounds( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_WEAPON();

  const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( weapon->clazz );

  weapon->nRounds = clamp( l_toint( 1 ), -1, weaponClazz->nRounds );
  return 1;
}

static int ozWeaponAddRounds( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_WEAPON();

  const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( weapon->clazz );

  if( weapon->nRounds >= 0 ) {
    weapon->nRounds = min( weapon->nRounds + l_toint( 1 ), weaponClazz->nRounds );
  }
  return 1;
}

/*
 * Bot
 */

static int ozBotGetName( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  l_pushstring( bot->name );
  return 1;
}

static int ozBotSetName( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  bot->name = l_tostring( 1 );
  return 0;
}

static int ozBotGetMind( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  l_pushstring( bot->mindFunc );
  return 1;
}

static int ozBotSetMind( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  bot->mindFunc = l_tostring( 1 );
  return 0;
}

static int ozBotHasState( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  l_pushbool( bot->state & l_toint( 1 ) );
  return 1;
}

static int ozBotGetEyePos( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  l_pushfloat( bot->p.x );
  l_pushfloat( bot->p.y );
  l_pushfloat( bot->p.z + bot->camZ );
  return 3;
}

static int ozBotGetH( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  l_pushfloat( Math::deg( bot->h ) );
  return 1;
}

static int ozBotSetH( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  bot->h = Math::rad( l_tofloat( 1 ) );
  bot->h = angleWrap( bot->h );
  return 0;
}

static int ozBotAddH( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  bot->h += Math::rad( l_tofloat( 1 ) );
  bot->h  = angleWrap( bot->h );
  return 0;
}

static int ozBotGetV( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  l_pushfloat( Math::deg( bot->v ) );
  return 1;
}

static int ozBotSetV( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  bot->v = Math::rad( l_tofloat( 1 ) );
  bot->v = clamp( bot->v, 0.0f, Math::TAU / 2.0f );
  return 0;
}

static int ozBotAddV( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  bot->v += Math::rad( l_tofloat( 1 ) );
  bot->v  = clamp( bot->v, 0.0f, Math::TAU / 2.0f );
  return 0;
}

static int ozBotGetDir( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( bot->h, &hvsc[0], &hvsc[1] );
  Math::sincos( bot->v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

  l_pushfloat( -hvsc[4] );
  l_pushfloat(  hvsc[5] );
  l_pushfloat( -hvsc[3] );

  return 3;
}

static int ozBotMaxStamina( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

  l_pushfloat( clazz->stamina );
  return 1;
}

static int ozBotGetStamina( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  l_pushfloat( bot->stamina );
  return 1;
}

static int ozBotSetStamina( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

  bot->stamina = clamp( l_tofloat( 1 ), 0.0f, clazz->stamina );
  return 0;
}

static int ozBotAddStamina( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

  bot->stamina = clamp( bot->stamina + l_tofloat( 1 ), 0.0f, clazz->stamina );
  return 0;
}

static int ozBotGetCargo( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  const Object* cargo = orbis.obj( bot->cargo );

  l_pushint( cargo == nullptr ? -1 : bot->cargo );
  return 1;
}

static int ozBotGetWeapon( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  const Object* weapon = orbis.obj( bot->weapon );

  l_pushint( weapon == nullptr ? -1 : bot->weapon );
  return 1;
}

static int ozBotSetWeaponItem( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  int item = l_toint( 1 );
  if( item < 0 ) {
    bot->weapon = -1;
  }
  else {
    if( uint( item ) >= uint( bot->items.length() ) ) {
      ERROR( "Invalid item number (out of range)" );
    }

    int index = bot->items[item];
    Weapon* weapon = static_cast<Weapon*>( orbis.obj( index ) );

    if( weapon == nullptr ) {
      l_pushbool( false );
      return 1;
    }

    if( !( weapon->flags & Object::WEAPON_BIT ) ) {
      ERROR( "Invalid item number (not a weapon)" );
    }

    if( !bot->canEquip( weapon ) ) {
      l_pushbool( false );
      return 1;
    }
    else {
      bot->weapon = index;
    }
  }

  l_pushbool( true );
  return 1;
}

static int ozBotCanReachEntity( lua_State* l )
{
  ARG( 2 );
  OBJ();
  OBJ_BOT();
  STR_INDEX( l_toint( 1 ) );
  ENT_INDEX( l_toint( 2 ) );

  l_pushbool( bot->canReach( ent ) );
  return 1;
}

static int ozBotCanReachObj( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();
  OBJ_INDEX( l_toint( 1 ) );

  l_pushbool( bot->canReach( obj ) );
  return 1;
}

static int ozBotAction( lua_State* l )
{
  VARG( 1, 3 );
  OBJ();
  OBJ_BOT();

  int action = l_toint( 1 );
  int arg1   = l_toint( 2 );
  int arg2   = l_toint( 3 );

  if( action & Bot::INSTRUMENT_ACTIONS ) {
    bot->actions   &= ~Bot::INSTRUMENT_ACTIONS;
    bot->actions   |= action;
    bot->instrument = arg1;
    bot->container  = arg2;
  }
  else {
    bot->actions |= action;
  }
  return 0;
}

static int ozBotHeal( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  bot->heal();
  return 0;
}

static int ozBotRearm( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  bot->rearm();
  return 0;
}

static int ozBotKill( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  bot->kill();
  return 0;
}

/*
 * Vehicle
 */

static int ozVehicleGetPilot( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_VEHICLE();

  Object* pilot = orbis.obj( veh->pilot );

  l_pushint( pilot == nullptr ? -1 : veh->pilot );
  return 1;
}

static int ozVehicleGetH( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_VEHICLE();

  l_pushfloat( Math::deg( veh->h ) );
  return 1;
}

static int ozVehicleSetH( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_VEHICLE();

  veh->h = Math::rad( l_tofloat( 1 ) );
  veh->h = angleWrap( veh->h );

  veh->rot = Mat44::rotationZXZ( veh->h, veh->v - Math::TAU / 4.0f, 0.0f );
  return 0;
}

static int ozVehicleAddH( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_VEHICLE();

  veh->h += Math::rad( l_tofloat( 1 ) );
  veh->h  = angleWrap( veh->h );

  veh->rot = Mat44::rotationZXZ( veh->h, veh->v - Math::TAU / 4.0f, 0.0f );
  return 0;
}

static int ozVehicleGetV( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_VEHICLE();

  l_pushfloat( Math::deg( veh->v ) );
  return 1;
}

static int ozVehicleSetV( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_VEHICLE();

  veh->v = Math::rad( l_tofloat( 1 ) );
  veh->v = clamp( veh->v, 0.0f, Math::TAU / 2.0f );

  veh->rot = Mat44::rotationZXZ( veh->h, veh->v - Math::TAU / 4.0f, 0.0f );
  return 0;
}

static int ozVehicleAddV( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_VEHICLE();

  veh->v += Math::rad( l_tofloat( 1 ) );
  veh->v  = clamp( veh->v, 0.0f, Math::TAU / 2.0f );

  veh->rot = Mat44::rotationZXZ( veh->h, veh->v - Math::TAU / 4.0f, 0.0f );
  return 0;
}

static int ozVehicleGetDir( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_VEHICLE();

  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( veh->h, &hvsc[0], &hvsc[1] );
  Math::sincos( veh->v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

  l_pushfloat( -hvsc[4] );
  l_pushfloat(  hvsc[5] );
  l_pushfloat( -hvsc[3] );

  return 3;
}

static int ozVehicleEmbarkBot( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_VEHICLE();

  if( veh->pilot >= 0 ) {
    ERROR( "Vehicle already has a pilot" );
  }

  BOT_INDEX( l_toint( 1 ) );

  if( bot->cell == nullptr ) {
    ERROR( "Bot is already in some vehicle" );
  }

  veh->pilot = bot->index;
  bot->enter( veh->index );
  return 0;
}

static int ozVehicleDisembarkBot( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_VEHICLE();

  Bot* pilot = static_cast<Bot*>( orbis.obj( veh->pilot ) );
  if( pilot == nullptr ) {
    return 0;
  }

  pilot->exit();
  return 0;
}

static int ozVehicleService( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_VEHICLE();

  veh->service();
  return 0;
}

/*
 * Fragment
 */

static int ozFragBindIndex( lua_State* l )
{
  ARG( 1 );

  int index = l_toint( 1 );
  if( uint( index ) >= uint( Orbis::MAX_FRAGS ) ) {
    ms.frag = nullptr;
  }
  else {
    ms.frag = orbis.frag( index );
  }

  l_pushbool( ms.frag != nullptr );
  return 1;
}

static int ozFragIsNull( lua_State* l )
{
  ARG( 0 );

  l_pushbool( ms.frag == nullptr );
  return 1;
}

static int ozFragGetIndex( lua_State* l )
{
  ARG( 0 );

  l_pushint( ms.frag == nullptr ? -1 : ms.frag->index );
  return 1;
}

static int ozFragGetPos( lua_State* l )
{
  ARG( 0 );
  FRAG();

  l_pushfloat( ms.frag->p.x );
  l_pushfloat( ms.frag->p.y );
  l_pushfloat( ms.frag->p.z );
  return 3;
}

static int ozFragWarpPos( lua_State* l )
{
  ARG( 3 );
  FRAG();

  ms.frag->p.x = l_tofloat( 1 );
  ms.frag->p.y = l_tofloat( 2 );
  ms.frag->p.z = l_tofloat( 3 );
  return 0;
}

static int ozFragGetVelocity( lua_State* l )
{
  ARG( 0 );
  FRAG();

  l_pushfloat( ms.frag->velocity.x );
  l_pushfloat( ms.frag->velocity.y );
  l_pushfloat( ms.frag->velocity.z );
  return 3;
}

static int ozFragSetVelocity( lua_State* l )
{
  ARG( 3 );
  FRAG();

  ms.frag->velocity.x = l_tofloat( 1 );
  ms.frag->velocity.y = l_tofloat( 2 );
  ms.frag->velocity.z = l_tofloat( 3 );
  return 0;
}

static int ozFragAddVelocity( lua_State* l )
{
  ARG( 3 );
  FRAG();

  ms.frag->velocity.x += l_tofloat( 1 );
  ms.frag->velocity.y += l_tofloat( 2 );
  ms.frag->velocity.z += l_tofloat( 3 );
  return 0;
}

static int ozFragGetLife( lua_State* l )
{
  ARG( 0 );
  FRAG();

  l_pushfloat( ms.frag->life );
  return 1;
}

static int ozFragSetLife( lua_State* l )
{
  ARG( 1 );
  FRAG();

  ms.frag->life = l_tofloat( 1 );
  return 0;
}

static int ozFragAddLife( lua_State* l )
{
  ARG( 1 );
  FRAG();

  ms.frag->life += l_tofloat( 1 );
  return 0;
}

static int ozFragRemove( lua_State* l )
{
  ARG( 0 );
  FRAG();

  synapse.remove( ms.frag );
  ms.frag = nullptr;
  return 0;
}

static int ozFragOverlaps( lua_State* l )
{
  ARG( 2 );
  FRAG();

  int   flags = l_toint( 1 );
  float dim   = l_tofloat( 2 );
  AABB  aabb  = AABB( ms.frag->p, Vec3( dim, dim, dim ) );

  hard_assert( collider.mask == Object::SOLID_BIT );

  if( flags & COLLIDE_ALL_OBJECTS_BIT ) {
    collider.mask = ~0;
  }

  bool overlaps = collider.overlaps( aabb );
  collider.mask = Object::SOLID_BIT;

  l_pushbool( overlaps );
  return 1;
}

static int ozFragBindOverlaps( lua_State* l )
{
  ARG( 2 );
  FRAG();

  int   flags = l_toint( 1 );
  float dim   = l_tofloat( 2 );
  AABB  aabb  = AABB( ms.frag->p, Vec3( dim, dim, dim ) );

  if( !( flags & ( COLLIDE_STRUCTS_BIT | COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) ) {
    ERROR( "At least one of OZ_STRUCTS_BIT, OZ_OBJECTS_BIT or OZ_ALL_OBJECTS_BIT must be given" );
  }

  List<Struct*>* structs = nullptr;
  List<Object*>* objects = nullptr;

  hard_assert( collider.mask == Object::SOLID_BIT );

  if( flags & COLLIDE_STRUCTS_BIT ) {
    structs = &ms.structs;

    ms.strIndex = 0;
    ms.structs.clear();
  }
  if( flags & ( COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) {
    objects = &ms.objects;

    ms.objIndex = 0;
    ms.objects.clear();
  }
  if( flags & COLLIDE_ALL_OBJECTS_BIT ) {
    collider.mask = ~0;
  }

  collider.getOverlaps( aabb, structs, objects );
  collider.mask = Object::SOLID_BIT;
  return 0;
}

static int ozFragVectorFromSelf( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF();

  Vec3 vec = ms.frag->p - ms.self->p;

  l_pushfloat( vec.x );
  l_pushfloat( vec.y );
  l_pushfloat( vec.z );
  return 3;
}

static int ozFragVectorFromSelfEye( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  vec = ms.frag->p - eye;

  l_pushfloat( vec.x );
  l_pushfloat( vec.y );
  l_pushfloat( vec.z );
  return 3;
}

static int ozFragDirFromSelf( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF();

  Vec3 dir = ~( ms.frag->p - ms.self->p );

  l_pushfloat( dir.x );
  l_pushfloat( dir.y );
  l_pushfloat( dir.z );
  return 3;
}

static int ozFragDirFromSelfEye( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  dir = ~( ms.frag->p - eye );

  l_pushfloat( dir.x );
  l_pushfloat( dir.y );
  l_pushfloat( dir.z );
  return 3;
}

static int ozFragDistFromSelf( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF();

  l_pushfloat( !( ms.frag->p - ms.self->p ) );
  return 1;
}

static int ozFragDistFromSelfEye( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );

  l_pushfloat( !( ms.frag->p - eye ) );
  return 1;
}

static int ozFragHeadingFromSelfEye( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF_BOT();

  float dx    = ms.frag->p.x - self->p.x;
  float dy    = ms.frag->p.y - self->p.y;
  float angle = Math::deg( angleWrap( Math::atan2( -dx, dy ) ) );

  l_pushfloat( angle );
  return 1;
}

static int ozFragRelHeadingFromSelfEye( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF_BOT();

  float dx    = ms.frag->p.x - self->p.x;
  float dy    = ms.frag->p.y - self->p.y;
  float angle = Math::deg( angleDiff( Math::atan2( -dx, dy ), self->h ) );

  l_pushfloat( angle );
  return 1;
}

static int ozFragPitchFromSelfEye( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF_BOT();

  Point eye   = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  float dx    = ms.frag->p.x - eye.x;
  float dy    = ms.frag->p.y - eye.y;
  float dz    = ms.frag->p.z - eye.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  l_pushfloat( angle );
  return 1;
}

static int ozFragIsVisibleFromSelf( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF();

  Vec3 vec = ms.frag->p - ms.self->p;

  collider.translate( ms.self->p, vec, ms.self );

  l_pushbool( collider.hit.ratio == 1.0f );
  return 1;
}

static int ozFragIsVisibleFromSelfEye( lua_State* l )
{
  ARG( 0 );
  FRAG();
  SELF_BOT();

  Point eye = Point( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3  vec = ms.frag->p - eye;

  collider.translate( eye, vec, self );

  l_pushbool( collider.hit.ratio == 1.0f );
  return 1;
}

/// @}

/**
 * Register matrix-specific Lua constants with a given Lua VM.
 */
void importMatrixConstants( lua_State* l );

}
