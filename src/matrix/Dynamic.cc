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
 * @file matrix/Dynamic.cc
 */

#include <matrix/Dynamic.hh>

#include <matrix/LuaMatrix.hh>
#include <matrix/Synapse.hh>

namespace oz
{

Pool<Dynamic, 4096> Dynamic::pool;

void Dynamic::onDestroy()
{
  if( !clazz->onDestroy.isEmpty() ) {
    luaMatrix.objectCall( clazz->onDestroy, this );
  }

  foreach( i, items.citer() ) {
    Dynamic* item = static_cast<Dynamic*>( orbis.objects[*i] );

    if( item != nullptr ) {
      item->destroy();
    }
  }

  if( cell != nullptr && clazz->fragPool != nullptr ) {
    synapse.gen( clazz->fragPool,
                 clazz->nFrags,
                 Bounds( Point( p.x - dim.x, p.y - dim.y, p.z ),
                         Point( p.x + dim.x, p.y + dim.y, p.z + dim.z ) ),
                 velocity + DESTRUCT_FRAG_VELOCITY );
  }
}

Dynamic::Dynamic( const DynamicClass* clazz_, int index_, const Point& p_, Heading heading ) :
  Object( clazz_, index_, p_, heading )
{
  velocity = Vec3::ZERO;
  momentum = Vec3::ZERO;
  floor    = Vec3( 0.0f, 0.0f, 1.0f );
  parent   = -1;
  lower    = -1;
  depth    = 0.0f;
  mass     = clazz_->mass;
  lift     = clazz_->lift;
}

Dynamic::Dynamic( const DynamicClass* clazz_, InputStream* istream ) :
  Object( clazz_, istream )
{
  velocity = istream->readVec3();
  momentum = istream->readVec3();
  floor    = istream->readVec3();
  parent   = istream->readInt();
  lower    = istream->readInt();
  depth    = istream->readFloat();
  mass     = clazz_->mass;
  lift     = clazz_->lift;
}

Dynamic::Dynamic( const DynamicClass* clazz_, const JSON& json ) :
  Object( clazz_, json )
{
  velocity = json["velocity"].asVec3();
  momentum = json["momentum"].asVec3();
  floor    = json["floor"].asVec3();
  parent   = json["parent"].asInt();
  lower    = json["lower"].asInt();
  depth    = json["depth"].asFloat();
  mass     = clazz_->mass;
  lift     = clazz_->lift;
}

void Dynamic::write( OutputStream* ostream ) const
{
  Object::write( ostream );

  ostream->writeVec3( velocity );
  ostream->writeVec3( momentum );
  ostream->writeVec3( floor );
  ostream->writeInt( parent );
  ostream->writeInt( lower );
  ostream->writeFloat( depth );
}

JSON Dynamic::write() const
{
  JSON json = Object::write();

  json.add( "velocity", velocity );
  json.add( "momentum", momentum );
  json.add( "floor", floor );
  json.add( "parent", orbis.objIndex( parent ) );
  json.add( "lower", orbis.objIndex( lower ) );
  json.add( "depth", depth );

  return json;
}

void Dynamic::readUpdate( InputStream* )
{}

void Dynamic::writeUpdate( OutputStream* ) const
{}

}
