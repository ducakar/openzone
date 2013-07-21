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
 * @file nirvana/TechTree.cc
 */

#include <nirvana/TechTree.hh>

#include <matrix/Liber.hh>
#include <common/Lingua.hh>

namespace oz
{

TechTree::Node* TechTree::findNode( const char* name )
{
  foreach( node, nodes.iter() ) {
    if( node->name.equals( name ) ) {
      return node;
    }
  }

  OZ_ERROR( "Invalid TechTree node reference '%s'", name );
}

void TechTree::update()
{
  if( allowedBuildings.isEmpty() ) {
    foreach( node, nodes.citer() ) {
      if( node->type == Node::BUILDING ) {
        allowedBuildings.add( node->building );
      }
    }
  }

  if( allowedUnits.isEmpty() ) {
    foreach( node, nodes.citer() ) {
      if( node->type == Node::UNIT ) {
        allowedUnits.add( node->unit );
      }
    }
  }

  if( allowedItems.isEmpty() ) {
    foreach( node, nodes.citer() ) {
      if( node->type == Node::ITEM ) {
        allowedItems.add( node->item );
      }
    }
  }
}

void TechTree::read( InputStream* istream )
{
  foreach( node, nodes.iter() ) {
    node->progress = istream->readFloat();
  }
}

void TechTree::write( OutputStream* ostream ) const
{
  foreach( node, nodes.citer() ) {
    ostream->writeFloat( node->progress );
  }
}

void TechTree::load()
{
  File configFile = "@nirvana/techTree.json";
  JSON config;
  if( !config.load( configFile ) ) {
    return;
  }

  int nNodes = config.length();
  for( int i = 0; i < nNodes; ++i ) {
    const JSON& tech = config[i];

    nodes.add();
    Node& node = nodes.last();

    const char* technology = tech["technology"].get( "" );
    const char* building   = tech["building"].get( "" );
    const char* unit       = tech["unit"].get( "" );
    const char* item       = tech["item"].get( "" );

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
      node.type = Node::UNIT;
      node.name = unit;
      node.unit = liber.objClass( node.name );
    }
    else if( !String::isEmpty( item ) ) {
      node.type = Node::ITEM;
      node.name = item;
      node.item = liber.objClass( node.name );
    }
    else {
      OZ_ERROR( "Tech node must have either 'technology', 'building', 'unit' or 'item' defined" );
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
      Node* depNode = findNode( requires[i].asString() );

      node.requires.add( depNode );
    }
  }

  config.clear( true );
}

void TechTree::unload()
{
  allowedBuildings.clear();
  allowedBuildings.deallocate();

  allowedUnits.clear();
  allowedUnits.deallocate();

  allowedItems.clear();
  allowedItems.deallocate();

  nodes.clear();
  nodes.deallocate();
}

TechTree techTree;

}
