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
 * @file matrix/luaapi.hh
 *
 * Matrix Lua API implementation.
 */

#pragma once

#include "common/luaapi.hh"

#include "matrix/Library.hh"
#include "matrix/Vehicle.hh"
#include "matrix/Physics.hh"
#include "matrix/Synapse.hh"

namespace oz
{
namespace matrix
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
  Object*         self;
  Bot*            user;

  Struct*         str;
  Entity*         ent;
  Object*         obj;
  Frag*           frag;

  int             strIndex;
  int             objIndex;

  Vector<Struct*> structs;
  Vector<Object*> objects;

  bool            hasUseFailed;
};

static MatrixLuaState ms;

/// @addtogroup luaapi
/// @{

/*
 * General functions
 */

static int ozUseFailed( lua_State* l )
{
  ARG( 0 );

  ms.hasUseFailed = true;
  return 1;
}

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
  VARG( 5, 6 );

  const BSP* bsp = null;
  try {
    bsp = library.bsp( l_tostring( 2 ) );
  }
  catch( const Exception& e ) {
    ERROR( e.what() );
  }

  AddMode mode    = AddMode( l_toint( 1 ) );
  Point3  p       = Point3( l_tofloat( 3 ), l_tofloat( 4 ), l_tofloat( 5 ) );
  Heading heading = Heading( l_gettop() == 6 ? l_toint( 6 ) : Math::rand( 4 ) );

  if( mode != ADD_FORCE ) {
    Bounds bounds = *bsp;
    bounds = Struct::rotate( bounds, heading ) + ( p - Point3::ORIGIN );

    if( collider.overlaps( bounds.toAABB() ) ) {
      ms.str = null;
      l_pushint( -1 );
      return 1;
    }
  }

  ms.str = synapse.add( bsp, p, heading );
  l_pushint( ms.str == null ? -1 : ms.str->index );
  return 1;
}

static int ozOrbisAddObj( lua_State* l )
{
  VARG( 5, 6 );

  const ObjectClass* clazz = null;
  try {
    clazz = library.objClass( l_tostring( 2 ) );
  }
  catch( const Exception& e ) {
    ERROR( e.what() );
  }

  AddMode mode    = AddMode( l_toint( 1 ) );
  Point3  p       = Point3( l_tofloat( 3 ), l_tofloat( 4 ), l_tofloat( 5 ) );
  Heading heading = Heading( l_gettop() == 6 ? l_toint( 6 ) : Math::rand( 4 ) );

  if( mode != ADD_FORCE ) {
    AABB aabb = AABB( p, clazz->dim );

    if( heading & WEST_EAST_MASK ) {
      swap( aabb.dim.x, aabb.dim.y );
    }

    if( collider.overlaps( aabb ) ) {
      ms.obj = null;
      l_pushint( -1 );
      return 1;
    }
  }

  ms.obj = synapse.add( clazz, p, heading );
  l_pushint( ms.obj == null ? -1 : ms.obj->index );
  return 1;
}

static int ozOrbisAddFrag( lua_State* l )
{
  ARG( 8 );

  const FragPool* pool = null;
  try {
    pool = library.fragPool( l_tostring( 2 ) );
  }
  catch( const Exception& e ) {
    ERROR( e.what() );
  }

  AddMode mode     = AddMode( l_toint( 1 ) );
  Point3  p        = Point3( l_tofloat( 3 ), l_tofloat( 4 ), l_tofloat( 5 ) );
  Vec3    velocity = Vec3( l_tofloat( 6 ), l_tofloat( 7 ), l_tofloat( 8 ) );

  if( mode != ADD_FORCE ) {
    if( collider.overlaps( p ) ) {
      ms.frag = null;
      l_pushint( -1 );
      return 1;
    }
  }

  ms.frag = synapse.add( pool, p, velocity );
  l_pushint( ms.frag == null ? -1 : ms.frag->index );
  return 1;
}

static int ozOrbisGenFrags( lua_State* l )
{
  ARG( 11 );

  const FragPool* pool = null;
  try {
    pool = library.fragPool( l_tostring( 1 ) );
  }
  catch( const Exception& e ) {
    ERROR( e.what() );
  }

  int    nFrags   = l_toint( 2 );
  Bounds bb       = Bounds( Point3( l_tofloat( 3 ), l_tofloat( 4 ), l_tofloat( 5 ) ),
                            Point3( l_tofloat( 6 ), l_tofloat( 7 ), l_tofloat( 8 ) ) );
  Vec3   velocity = Vec3( l_tofloat( 9 ), l_tofloat( 10 ), l_tofloat( 11 ) );

  synapse.gen( pool, nFrags, bb, velocity );
  ms.frag = null;
  return 0;
}

static int ozOrbisOverlaps( lua_State* l )
{
  VARG( 7, 8 );

  int  flags = l_toint( 1 );
  AABB aabb  = AABB( Point3( l_tofloat( 2 ), l_tofloat( 3 ), l_tofloat( 4 ) ),
                     Vec3( l_tofloat( 5 ), l_tofloat( 6 ), l_tofloat( 7 ) ) );

  const Object* exclObj = null;
  if( l_gettop() == 8 ) {
    int index = l_toint( 8 );

    if( uint( index ) >= uint( orbis.objects.length() ) ) {
      ERROR( "Invalid excluded object index (out of range)" );
    }

    exclObj = orbis.objects[index];
  }

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
  AABB aabb  = AABB( Point3( l_tofloat( 2 ), l_tofloat( 3 ), l_tofloat( 4 ) ),
                     Vec3( l_tofloat( 5 ), l_tofloat( 6 ), l_tofloat( 7 ) ) );

  if( !( flags & ( COLLIDE_STRUCTS_BIT | COLLIDE_OBJECTS_BIT | COLLIDE_ALL_OBJECTS_BIT ) ) ) {
    ERROR( "At least one of OZ_STRUCTS_BIT, OZ_OBJECTS_BIT or OZ_ALL_OBJECTS_BIT must be given" );
  }

  Vector<Struct*>* structs = null;
  Vector<Object*>* objects = null;

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
  int id = String::isEmpty( name ) ? -1 : library.caelumIndex( name );

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

/*
 * Terra
 */

static int ozTerraLoad( lua_State* l )
{
  ARG( 1 );

  const char* name = l_tostring( 1 );
  int id = String::isEmpty( name ) ? -1 : library.terraIndex( name );

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
  if( uint( index ) >= uint( orbis.structs.length() ) ) {
    ms.str = null;
  }
  else {
    ms.str = orbis.structs[index];
  }

  l_pushbool( ms.str != null );
  return 1;
}

static int ozBindNextStr( lua_State* l )
{
  ARG( 0 );

  ms.str = null;

  while( ms.str == null && ms.strIndex < ms.structs.length() ) {
    ms.str = ms.structs[ms.strIndex];
    ++ms.strIndex;
  }

  l_pushbool( ms.str != null );
  return 1;
}

static int ozStrIsNull( lua_State* l )
{
  ARG( 0 );

  l_pushbool( ms.str == null );
  return 1;
}

static int ozStrGetIndex( lua_State* l )
{
  ARG( 0 );

  l_pushint( ms.str == null ? -1 : ms.str->index );
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
  ms.str = null;
  return 0;
}

static int ozStrNumEnts( lua_State* l )
{
  ARG( 0 );
  STR();

  l_pushint( ms.str->nEntities );
  return 1;
}

static int ozStrBindEnt( lua_State* l )
{
  ARG( 1 );
  STR();

  int entIndex = l_toint( 1 );

  if( uint( entIndex ) >= uint( ms.str->nEntities ) ) {
    ERROR( "Invalid structure entity index (out of range)" );
  }

  ms.ent = &ms.str->entities[entIndex];
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
  BOT_INDEX( l_toint( 1 ) );

  Point3 eye = Point3( bot->p.x, bot->p.y, bot->p.z + bot->camZ );
  Vec3   vec = ms.str->p - eye;

  l_pushfloat( vec.x );
  l_pushfloat( vec.y );
  l_pushfloat( vec.z );
  return 3;
}

static int ozStrDirectionFromSelf( lua_State* l )
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

static int ozStrDirectionFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   dir = ~( ms.str->p - eye );

  l_pushfloat( dir.x );
  l_pushfloat( dir.y );
  l_pushfloat( dir.z );
  return 3;
}

static int ozStrDistanceFromSelf( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF();

  l_pushfloat( !( ms.str->p - ms.self->p ) );
  return 1;
}

static int ozStrDistanceFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

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
  float angle = Math::fmod( Math::deg( Math::atan2( -dx, dy ) ) + 360.0f, 360.0f );

  l_pushfloat( angle );
  return 1;
}

static int ozStrPitchFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR();
  SELF_BOT();

  Point3 eye   = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  float  dx    = ms.str->p.x - eye.x;
  float  dy    = ms.str->p.y - eye.y;
  float  dz    = ms.str->p.z - eye.z;
  float  angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  l_pushfloat( angle );
  return 1;
}

static int ozStrIsVisibleFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF();

  Vec3 vec = ms.str->p - ms.self->p;

  collider.translate( ms.self->p, vec, ms.self );

  l_pushbool( collider.hit.ratio == 1.0f || collider.hit.str == ms.str );
  return 1;
}

static int ozStrIsVisibleFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   vec = ms.str->p - eye;

  collider.translate( eye, vec, self );

  l_pushbool( collider.hit.ratio == 1.0f || collider.hit.str == ms.str );
  return 1;
}

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
    ms.ent->offset = ms.ent->model->move;
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
  STR();

  l_pushint( ms.ent->key );
  return 1;
}

static int ozEntSetLock( lua_State* l )
{
  ARG( 1 );
  STR();

  ms.ent->key = l_toint( 1 );
  return 0;
}

static int ozEntTrigger( lua_State* l )
{
  ARG( 0 );
  STR();

  ms.ent->trigger();
  return 0;
}

/*
 * Object
 */

static int ozBindObj( lua_State* l )
{
  ARG( 1 );

  int index = l_toint( 1 );
  if( uint( index ) >= uint( orbis.objects.length() ) ) {
    ms.obj = null;
  }
  else {
    ms.obj = orbis.objects[index];
  }

  l_pushbool( ms.obj != null );
  return 1;
}

static int ozBindSelf( lua_State* l )
{
  ARG( 0 );

  ms.obj = ms.self;

  l_pushbool( ms.obj != null );
  return 1;
}

static int ozBindUser( lua_State* l )
{
  ARG( 0 );

  ms.obj = ms.user;

  l_pushbool( ms.obj != null );
  return 1;
}

static int ozBindNextObj( lua_State* l )
{
  ARG( 0 );

  ms.obj = null;

  while( ms.obj == null && ms.objIndex < ms.objects.length() ) {
    ms.obj = ms.objects[ms.objIndex];
    ++ms.objIndex;
  }

  l_pushbool( ms.obj != null );
  return 1;
}

static int ozObjIsNull( lua_State* l )
{
  ARG( 0 );

  l_pushbool( ms.obj == null );
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

  l_pushbool( ms.obj != null && ms.obj->cell == null );
  return 1;
}

static int ozObjGetIndex( lua_State* l )
{
  ARG( 0 );

  l_pushint( ms.obj == null ? -1 : ms.obj->index );
  return 1;
}

static int ozObjGetPos( lua_State* l )
{
  ARG( 0 );
  OBJ();

  if( ms.obj->cell == null ) {
    hard_assert( ms.obj->flags & Object::DYNAMIC_BIT );

    const Dynamic* dyn = static_cast<const Dynamic*>( ms.obj );

    if( dyn->parent != -1 ) {
      Object* parent = orbis.objects[dyn->parent];

      if( parent != null ) {
        l_pushfloat( parent->p.x );
        l_pushfloat( parent->p.y );
        l_pushfloat( parent->p.z );
        return 3;
      }
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

  l_pushbool( ms.obj != null && ( ms.obj->flags & l_toint( 1 ) ) );
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

/**
 * Inflict damage to the object.
 *
 * @code void ozObjDamage( float damage ) @endcode
 *
 * If <tt>damage</tt> is greater than object's resistance, object receives that difference of damage
 * and <tt>EVENT_DAMAGE</tt> is generated.
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
 * %Object will be removed on the beginning of the next update. If <tt>quiet</tt> is true,
 * <tt>onDestroy</tt> handler will not be called nor <tt>EVENT_DESTROY</tt> will be generated.
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
    hard_assert( *item != -1 );

    ms.objects.add( orbis.objects[*item] );
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

  ms.obj = orbis.objects[ ms.obj->items[index] ];
  return 0;
}

static int ozObjAddItem( lua_State* l )
{
  ARG( 1 );
  OBJ();

  if( ms.obj->items.length() == ms.obj->clazz->nItems ) {
    l_pushbool( false );
    return 1;
  }

  ITEM_INDEX( l_toint( 1 ) );

  if( item->cell == null ) {
    hard_assert( item->parent != -1 );

    Object* container = orbis.objects[item->parent];
    if( container != null ) {
      container->items.exclude( item->index );
    }
  }

  item->parent = ms.obj->index;
  ms.obj->items.add( item->index );

  if( item->cell != null ) {
    synapse.cut( item );
  }

  l_pushbool( true );
  return 0;
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

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   vec = ms.obj->p - eye;

  l_pushfloat( vec.x );
  l_pushfloat( vec.y );
  l_pushfloat( vec.z );
  return 3;
}

static int ozObjDirectionFromSelf( lua_State* l )
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

static int ozObjDirectionFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   dir = ~( ms.obj->p - eye );

  l_pushfloat( dir.x );
  l_pushfloat( dir.y );
  l_pushfloat( dir.z );
  return 3;
}

static int ozObjDistanceFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF();

  l_pushfloat( !( ms.obj->p - ms.self->p ) );
  return 1;
}

static int ozObjDistanceFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

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
  float angle = Math::fmod( Math::deg( Math::atan2( -dx, dy ) ) + 360.0f, 360.0f );

  l_pushfloat( angle );
  return 1;
}

static int ozObjPitchFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  Point3 eye   = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  float  dx    = ms.obj->p.x - eye.x;
  float  dy    = ms.obj->p.y - eye.y;
  float  dz    = ms.obj->p.z - eye.z;
  float  angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

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

  l_pushbool( collider.hit.ratio == 1.0f || collider.hit.obj == ms.obj );
  return 1;
}

static int ozObjIsVisibleFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   vec = ms.obj->p - eye;

  collider.translate( eye, vec, self );

  l_pushbool( collider.hit.ratio == 1.0f || collider.hit.obj == ms.obj );
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

  l_pushint( dyn->parent != -1 && orbis.objects[dyn->parent] == null ? -1 : dyn->parent );
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

  if( weapon->nRounds != -1 ) {
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
  bot->h = Math::fmod( bot->h + Math::TAU, Math::TAU );
  return 0;
}

static int ozBotAddH( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  bot->h += Math::rad( l_tofloat( 1 ) );
  bot->h  = Math::fmod( bot->h + Math::TAU, Math::TAU );
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

static int ozBotGetCargo( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  l_pushint( bot->cargo != -1 && orbis.objects[bot->cargo] == null ? -1 : bot->cargo );
  return 1;
}

static int ozBotGetWeapon( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_BOT();

  l_pushint( bot->weapon != -1 && orbis.objects[bot->weapon] == null ? -1 : bot->weapon );
  return 1;
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

static int ozBotSetWeaponItem( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_BOT();

  int item = l_toint( 1 );
  if( item == -1 ) {
    bot->weapon = -1;
  }
  else {
    if( uint( item ) >= uint( ms.obj->items.length() ) ) {
      ERROR( "Invalid item number (out of range)" );
    }

    int index = ms.obj->items[item];
    Weapon* weapon = static_cast<Weapon*>( orbis.objects[index] );

    if( weapon == null ) {
      l_pushbool( false );
      return 1;
    }

    if( !( weapon->flags & Object::WEAPON_BIT ) ) {
      ERROR( "Invalid item number (not a weapon)" );
    }

    const WeaponClass* clazz = static_cast<const WeaponClass*>( weapon->clazz );
    if( bot->clazz->name.beginsWith( clazz->userBase ) ) {
      bot->weapon = index;
    }
  }

  l_pushbool( true );
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

static int ozBotCanReachEntity( lua_State* l )
{
  ARG( 2 );
  OBJ();
  OBJ_BOT();
  STR_INDEX( l_toint( 1 ) );

  int entIndex = l_toint( 2 );
  if( uint( entIndex ) >= uint( ms.str->nEntities ) ) {
    ERROR( "Invalid entity index (out of range)" );
  }

  l_pushbool( bot->canReach( &ms.str->entities[entIndex] ) );
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

/*
 * Vehicle
 */

static int ozVehicleGetPilot( lua_State* l )
{
  ARG( 0 );
  OBJ();
  OBJ_VEHICLE();

  l_pushint( veh->pilot != -1 && orbis.objects[veh->pilot] == null ? -1 : veh->pilot );
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
  veh->h = Math::fmod( veh->h + Math::TAU, Math::TAU );

  veh->rot = Quat::rotZXZ( veh->h, veh->v - Math::TAU / 4.0f, 0.0f );
  return 0;
}

static int ozVehicleAddH( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_VEHICLE();

  veh->h += Math::rad( l_tofloat( 1 ) );
  veh->h  = Math::fmod( veh->h + Math::TAU, Math::TAU );

  veh->rot = Quat::rotZXZ( veh->h, veh->v - Math::TAU / 4.0f, 0.0f );
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

  veh->rot = Quat::rotZXZ( veh->h, veh->v - Math::TAU / 4.0f, 0.0f );
  return 0;
}

static int ozVehicleAddV( lua_State* l )
{
  ARG( 1 );
  OBJ();
  OBJ_VEHICLE();

  veh->v += Math::rad( l_tofloat( 1 ) );
  veh->v  = clamp( veh->v, 0.0f, Math::TAU / 2.0f );

  veh->rot = Quat::rotZXZ( veh->h, veh->v - Math::TAU / 4.0f, 0.0f );
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

  if( veh->pilot != -1 ) {
    ERROR( "Vehicle already has a pilot" );
  }

  BOT_INDEX( l_toint( 1 ) );

  if( bot->cell == null ) {
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

  if( veh->pilot == -1 ) {
    return 0;
  }

  Bot* pilot = static_cast<Bot*>( orbis.objects[veh->pilot] );
  if( pilot == null ) {
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
  if( uint( index ) >= uint( orbis.frags.length() ) ) {
    ms.frag = null;
  }
  else {
    ms.frag = orbis.frags[index];
  }

  l_pushbool( ms.frag != null );
  return 1;
}

static int ozFragIsNull( lua_State* l )
{
  ARG( 0 );

  l_pushbool( ms.frag == null );
  return 1;
}

static int ozFragGetIndex( lua_State* l )
{
  ARG( 0 );

  l_pushint( ms.frag == null ? -1 : ms.frag->index );
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
  ms.frag = null;
  return 0;
}

/// @}

/**
 * Register matrix-specific %Lua constants with the given %Lua VM.
 *
 * @ingroup matrix
 */
void importLuaConstants( lua_State* l );

}
}
