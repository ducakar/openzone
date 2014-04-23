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

namespace oz
{

const float Object::REAL_MAX_DIM            = MAX_DIM - 0.01f;

const float Object::DAMAGE_BASE_INTENSITY   = 0.30f;
const float Object::DAMAGE_INTENSITY_COEF   = 0.01f;
const Vec3  Object::DESTRUCT_FRAG_VELOCITY  = Vec3( 0.0f, 0.0f, 2.0f );

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
    items.allocate( clazz->nItems );
  }
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
    items.allocate( clazz->nItems );

    int nItems = is->readInt();
    for( int i = 0; i < nItems; ++i ) {
      items.add( is->readInt() );
    }
  }
}

Object::Object( const ObjectClass* clazz_, const JSON& json )
{
  p          = json["p"].get( Point::ORIGIN );
  dim        = clazz_->dim;
  cell       = nullptr;
  index      = json["index"].get( -1 );
  flags      = json["flags"].get( 0 );
  life       = json["life"].get( 0.0f );
  resistance = clazz_->resistance;
  clazz      = clazz_;

  if( index < 0 ) {
    OZ_ERROR( "Invalid object index" );
  }

  if( flags & WEST_EAST_MASK ) {
    swap( dim.x, dim.y );
  }

  if( clazz->nItems != 0 ) {
    items.allocate( clazz->nItems );

    const JSON& itemsJSON = json["items"];

    int nItems = itemsJSON.length();
    for( int i = 0; i < nItems; ++i ) {
      int index = itemsJSON[i].get( -1 );
      if( index < 0 ) {
        OZ_ERROR( "Invalid item index" );
      }
      items.add( index );
    }
  }
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

JSON Object::write() const
{
  JSON json( JSON::OBJECT );

  json.add( "class", clazz->name );

  json.add( "p", p );
  json.add( "index", index );
  json.add( "flags", flags );
  json.add( "life", life );

  if( clazz->nItems != 0 ) {
    JSON& itemsJSON = json.add( "items", JSON::ARRAY );

    for( int item : items ) {
      itemsJSON.add( orbis.objIndex( item ) );
    }
  }

  return json;
}

void Object::readUpdate( InputStream* )
{}

void Object::writeUpdate( OutputStream* ) const
{}

}
