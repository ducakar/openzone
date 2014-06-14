/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file matrix/Object.cc
 */

#include <matrix/Object.hh>

#include <matrix/LuaMatrix.hh>
#include <matrix/Synapse.hh>

#define OZ_FLAG_READ( flagBit, name ) \
  if( flagJSON.get( String::EMPTY ).equals( name ) ) { \
    flags |= flagBit; \
  }

#define OZ_FLAG_WRITE( flagBit, name ) \
  if( flags & flagBit ) { \
    flagsJSON.add( name ); \
  }

namespace oz
{

const float Object::REAL_MAX_DIM           = float( MAX_DIM ) - 0.01f;
const float Object::DAMAGE_BASE_INTENSITY  = 0.30f;
const float Object::DAMAGE_INTENSITY_COEF  = 0.01f;
const Vec3  Object::DESTRUCT_FRAG_VELOCITY = Vec3( 0.0f, 0.0f, 2.0f );

Pool<Object::Event, 256> Object::Event::pool;
Pool<Object, 16384>      Object::pool;

void Object::onDestroy()
{
  hard_assert( cell != nullptr );

  if( !clazz->onDestroy.isEmpty() ) {
    luaMatrix.objectCall( clazz->onDestroy, this );
  }

  for( int i : items ) {
    Object* item = orbis.obj( i );

    if( item != nullptr ) {
      item->destroy();
    }
  }

  if( clazz->fragPool != nullptr ) {
    synapse.gen( clazz->fragPool,
                 clazz->nFrags,
                 Bounds( Point( p.x - dim.x, p.y - dim.y, p.z ),
                         Point( p.x + dim.x, p.y + dim.y, p.z + dim.z ) ),
                 DESTRUCT_FRAG_VELOCITY );
  }
}

bool Object::onUse( Bot* user )
{
  hard_assert( !clazz->onUse.isEmpty() );

  return luaMatrix.objectCall( clazz->onUse, this, user );
}

void Object::onUpdate()
{
  hard_assert( !clazz->onUpdate.isEmpty() );

  luaMatrix.objectCall( clazz->onUpdate, this );
}

String Object::getTitle() const
{
  return clazz->name;
}

float Object::getStatus() const
{
  hard_assert( !clazz->getStatus.isEmpty() );

  luaMatrix.objectCall( clazz->getStatus, const_cast<Object*>( this ) );
  return luaMatrix.objectStatus;
}

Object::~Object()
{
  hard_assert( dim.x <= REAL_MAX_DIM );
  hard_assert( dim.y <= REAL_MAX_DIM );

  events.free();
}

Object::Object( const ObjectClass* clazz_, int index_, const Point& p_, Heading heading )
{
  p          = p_;
  dim        = clazz_->dim;
  cell       = nullptr;
  index      = index_;
  flags      = clazz_->flags | heading;
  life       = clazz_->life;
  resistance = clazz_->resistance;
  clazz      = clazz_;

  if( flags & WEST_EAST_MASK ) {
    swap( dim.x, dim.y );
  }

  if( clazz->nItems != 0 ) {
    items.reserve( clazz->nItems );
  }
}

Object::Object( const ObjectClass* clazz_, int index_, const JSON& json )
{
  p          = json["p"].get( Point::ORIGIN );
  dim        = clazz_->dim;
  cell       = nullptr;
  index      = index_;
  flags      = 0;
  life       = json["life"].get( 0.0f );
  resistance = clazz_->resistance;
  clazz      = clazz_;

  for( const JSON& flagJSON : json["flags"].arrayCIter() ) {
    OZ_FLAG_READ( DYNAMIC_BIT,      "dynamic"   );
    OZ_FLAG_READ( WEAPON_BIT,       "weapon"    );
    OZ_FLAG_READ( BOT_BIT,          "bot"       );
    OZ_FLAG_READ( VEHICLE_BIT,      "vehicle"   );
    OZ_FLAG_READ( ITEM_BIT,         "item"      );
    OZ_FLAG_READ( BROWSABLE_BIT,    "browsable" );
    OZ_FLAG_READ( LUA_BIT,          "lua"       );
    OZ_FLAG_READ( DESTROY_FUNC_BIT, "onDestroy" );
    OZ_FLAG_READ( USE_FUNC_BIT,     "onUse"     );
    OZ_FLAG_READ( UPDATE_FUNC_BIT,  "onUpdate"  );
    OZ_FLAG_READ( STATUS_FUNC_BIT,  "getStatus" );
    OZ_FLAG_READ( DEVICE_BIT,       "device"    );
    OZ_FLAG_READ( IMAGO_BIT,        "imago"     );
    OZ_FLAG_READ( AUDIO_BIT,        "audio"     );
    OZ_FLAG_READ( SOLID_BIT,        "solid"     );
    OZ_FLAG_READ( CYLINDER_BIT,     "cylinder"  );
    OZ_FLAG_READ( WIDE_CULL_BIT,    "wideCull"  );
  }

  if( flags & WEST_EAST_MASK ) {
    swap( dim.x, dim.y );
  }

  items.reserve( clazz->nItems );
}

Object::Object( const ObjectClass* clazz_, InputStream* is )
{
  p          = is->readPoint();
  dim        = clazz_->dim;
  cell       = nullptr;
  index      = is->readInt();
  flags      = is->readInt();
  life       = is->readFloat();
  resistance = clazz_->resistance;
  clazz      = clazz_;

  if( flags & WEST_EAST_MASK ) {
    swap( dim.x, dim.y );
  }

  int nEvents = is->readInt();
  for( int i = 0; i < nEvents; ++i ) {
    int   id        = is->readInt();
    float intensity = is->readFloat();

    addEvent( id, intensity );
  }

  if( clazz->nItems != 0 ) {
    items.reserve( clazz->nItems );

    int nItems = is->readInt();
    for( int i = 0; i < nItems; ++i ) {
      items.add( is->readInt() );
    }
  }
}

JSON Object::write() const
{
  JSON json( JSON::OBJECT );

  json.add( "class", clazz->name );
  json.add( "life", life );

  if( cell != nullptr ) {
    json.add( "p", p );
  }

  JSON& flagsJSON = json.add( "flags", JSON::ARRAY );

  OZ_FLAG_WRITE( DYNAMIC_BIT,      "dynamic"   );
  OZ_FLAG_WRITE( WEAPON_BIT,       "weapon"    );
  OZ_FLAG_WRITE( BOT_BIT,          "bot"       );
  OZ_FLAG_WRITE( VEHICLE_BIT,      "vehicle"   );
  OZ_FLAG_WRITE( ITEM_BIT,         "item"      );
  OZ_FLAG_WRITE( BROWSABLE_BIT,    "browsable" );
  OZ_FLAG_WRITE( LUA_BIT,          "lua"       );
  OZ_FLAG_WRITE( DESTROY_FUNC_BIT, "onDestroy" );
  OZ_FLAG_WRITE( USE_FUNC_BIT,     "onUse"     );
  OZ_FLAG_WRITE( UPDATE_FUNC_BIT,  "onUpdate"  );
  OZ_FLAG_WRITE( STATUS_FUNC_BIT,  "getStatus" );
  OZ_FLAG_WRITE( DEVICE_BIT,       "device"    );
  OZ_FLAG_WRITE( IMAGO_BIT,        "imago"     );
  OZ_FLAG_WRITE( AUDIO_BIT,        "audio"     );
  OZ_FLAG_WRITE( SOLID_BIT,        "solid"     );
  OZ_FLAG_WRITE( CYLINDER_BIT,     "cylinder"  );
  OZ_FLAG_WRITE( WIDE_CULL_BIT,    "wideCull"  );

  switch( flags & HEADING_MASK ) {
    case NORTH: {
      flagsJSON.add( "north" );
      break;
    }
    case WEST: {
      flagsJSON.add( "west" );
      break;
    }
    case SOUTH: {
      flagsJSON.add( "south" );
      break;
    }
    case EAST: {
      flagsJSON.add( "east" );
      break;
    }
    default: {
      hard_assert( false );
      break;
    }
  }

  if( clazz->nItems != 0 ) {
    JSON& itemsJSON = json.add( "items", JSON::ARRAY );

    for( int index : items ) {
      const Object* item = orbis.obj( index );

      if( item != nullptr ) {
        itemsJSON.add( item->write() );
      }
    }
  }

  return json;
}

void Object::write( OutputStream* os ) const
{
  os->writePoint( p );
  os->writeInt( index );
  os->writeInt( flags );
  os->writeFloat( life );

  os->writeInt( events.length() );
  for( const Event& event : events ) {
    os->writeInt( event.id );
    os->writeFloat( event.intensity );
  }

  if( clazz->nItems != 0 ) {
    os->writeInt( items.length() );
    for( int item : items ) {
      os->writeInt( item );
    }
  }
}

void Object::readUpdate( InputStream* )
{}

void Object::writeUpdate( OutputStream* ) const
{}

}
