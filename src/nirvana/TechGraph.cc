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
 * @file nirvana/TechGraph.cc
 */

#include <nirvana/TechGraph.hh>

#include <matrix/Liber.hh>
#include <common/Lingua.hh>

namespace oz
{

TechGraph::Node* TechGraph::findNode( const char* name )
{
  for( Node& node : nodes ) {
    if( node.name.equals( name ) ) {
      return &node;
    }
  }

  OZ_ERROR( "Invalid TechGraph node reference '%s'", name );
}

bool TechGraph::enable( const char* )
{
  return false;
}

bool TechGraph::disable( const char* )
{
  return false;
}

void TechGraph::enableAll()
{
  for( Node& node : nodes ) {
    node.progress = 1.0f;

    if( node.type == Node::BUILDING ) {
      allowedBuildings.add( node.building );
    }
    else if( node.type == Node::UNIT ) {
      allowedUnits.add( node.object );
    }
    else if( node.type == Node::ITEM ) {
      allowedItems.add( node.object );
    }
    else if( node.type == Node::OBJECT ) {
      allowedObjects.add( node.object );
    }
  }
}

void TechGraph::disableAll()
{
  for( Node& node : nodes ) {
    node.progress = 0.0f;
  }

  allowedBuildings.clear();
  allowedBuildings.trim();

  allowedUnits.clear();
  allowedUnits.trim();

  allowedItems.clear();
  allowedItems.trim();

  allowedObjects.clear();
  allowedObjects.trim();
}

void TechGraph::update()
{}

void TechGraph::read( InputStream* is )
{
  for( Node& node : nodes ) {
    node.progress = is->readFloat();
  }
}

void TechGraph::write( OutputStream* os ) const
{
  for( const Node& node : nodes ) {
    os->writeFloat( node.progress );
  }
}

void TechGraph::load()
{
  File configFile = "@nirvana/techGraph.json";
  JSON config;
  if( !config.load( configFile ) ) {
    return;
  }

  int nNodes = config.length();
  for( int i = 0; i < nNodes; ++i ) {
    const JSON& tech = config[i];

    nodes.add( {} );
    Node& node = nodes.last();

    const char* technology = tech["technology"].get( "" );
    const char* building   = tech["building"].get( "" );
    const char* unit       = tech["unit"].get( "" );
    const char* item       = tech["item"].get( "" );
    const char* object     = tech["object"].get( "" );

    if( !String::isEmpty( technology ) ) {
      node.type     = Node::TECHNOLOGY;
      node.name     = technology;
      node.building = nullptr;
    }
    else if( !String::isEmpty( building ) ) {
      node.type     = Node::BUILDING;
      node.name     = building;
      node.building = liber.bsp( node.name );
    }
    else if( !String::isEmpty( unit ) ) {
      node.type   = Node::UNIT;
      node.name   = unit;
      node.object = liber.objClass( node.name );
    }
    else if( !String::isEmpty( item ) ) {
      node.type   = Node::ITEM;
      node.name   = item;
      node.object = liber.objClass( node.name );
    }
    else if( !String::isEmpty( object ) ) {
      node.type   = Node::OBJECT;
      node.name   = object;
      node.object = liber.objClass( node.name );
    }
    else {
      OZ_ERROR( "Tech node must have either 'technology', 'building', 'unit', 'item' or 'object'"
                " defined" );
    }

    node.title       = lingua.get( tech["title"].get( node.name ) );
    node.description = lingua.get( tech["description"].get( "" ) );
    node.price       = tech["price"].get( 0 );
    node.time        = tech["time"].get( 60.0f );
    node.progress    = node.price == 0 ? 1.0f : 0.0f;
  }

  for( int i = 0; i < nNodes; ++i ) {
    const JSON& tech     = config[i];
    const JSON& requires = tech["requires"];
    Node&       node     = nodes[i];

    if( requires.length() > Node::MAX_DEPS ) {
      OZ_ERROR( "Only %d dependencies per technology supported.", Node::MAX_DEPS );
    }

    int nRequires = requires.length();
    for( int i = 0; i < nRequires; ++i ) {
      Node* depNode = findNode( requires[i].get( "?" ) );

      node.requires.add( depNode );
    }
  }

  config.clear( true );

  enableAll();
}

void TechGraph::unload()
{
  allowedBuildings.clear();
  allowedBuildings.trim();

  allowedUnits.clear();
  allowedUnits.trim();

  allowedItems.clear();
  allowedItems.trim();

  allowedObjects.clear();
  allowedObjects.trim();

  nodes.clear();
  nodes.trim();
}

TechGraph techGraph;

}
