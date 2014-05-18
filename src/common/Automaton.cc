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
 * @file common/Automaton.cc
 */

#include <common/Automaton.hh>

#include <common/LuaCommon.hh>

namespace oz
{

Automaton::Automaton( const File& file, const LuaCommon* lua )
{
  name = file.baseName();

  JSON json = file;

  for( const JSON& jsonState : json.arrayCIter() ) {
    states.add( State() );
    State& state = states.last();

    const char* enterCode  = jsonState["onEnter"].get( "" );
    const char* updateCode = jsonState["onUpdate"].get( "" );

    state.name = jsonState["name"].get( "" );

    if( !String::isEmpty( enterCode ) ) {
      state.onEnter = lua->compile( enterCode, name );
    }
    if( !String::isEmpty( updateCode ) ) {
      state.onUpdate = lua->compile( updateCode, name );
    }
  }

  for( int i = 0; i < states.length(); ++i ) {
    const JSON& jsonState = json[i];
    State& state = states[i];

    for( const JSON& jsonLink : jsonState["links"].arrayCIter() ) {
      const char* linkName = jsonLink["to"].get( "" );

      state.links.add( State::Link() );
      State::Link& link = state.links.last();

      link.condition = lua->compile( jsonLink["if"].get( "" ), name );
      link.target    = nullptr;

      for( const State& state : states ) {
        if( state.name.equals( linkName ) ) {
          link.target = &state;
          break;
        }
      }
      if( link.target == nullptr ) {
        OZ_ERROR( "Unable to find state '%s' in automaton '%s'", linkName, name.cstr() );
      }
    }
  }

  hard_assert( !states.isEmpty() );
}

const Automaton::State* Automaton::findState( const char *name ) const
{
  if( String::isEmpty( name ) ) {
    return &states[0];
  }

  for( const State& state : states ) {
    if( state.name.equals( name ) ) {
      return &state;
    }
  }
  return nullptr;
}

}
