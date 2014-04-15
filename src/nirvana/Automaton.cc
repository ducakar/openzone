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
 * @file nirvana/Automaton.cc
 */

#include <nirvana/Automaton.hh>

#include <nirvana/LuaNirvana.hh>

namespace oz
{

struct Automaton::State
{
  struct Link
  {
    Buffer condition;
    State* target;
  };

  String     name;
  Buffer     onEnter;
  Buffer     onUpdate;
  List<Link> links;
};

Automaton::State* Automaton::findState( const char* stateName )
{
  foreach( state, states.iter() ) {
    if( state->name.equals( stateName ) ) {
      return state;
    }
  }
  OZ_ERROR( "Unable to find state '%s' in automaton '%s'", stateName, name.cstr() );
}

Automaton::Automaton( const File& file )
{
  name = file.baseName();

  JSON json = file;

  foreach( i, json.arrayCIter() ) {
    const JSON& jsonState = *i;

    states.add();
    State& state = states.last();

    const char* enterCode  = jsonState["onEnter"].get( "" );
    const char* updateCode = jsonState["onUpdate"].get( "" );

    state.name = jsonState["name"].get( "" );

    if( !String::isEmpty( enterCode ) ) {
      state.onEnter = luaNirvana.compile( enterCode );
    }
    if( !String::isEmpty( updateCode ) ) {
      state.onUpdate = luaNirvana.compile( updateCode );
    }
  }

  for( int i = 0; i < states.length(); ++i ) {
    const JSON& jsonState = json[i];
    State& state = states[i];

    foreach( j, jsonState["links"].arrayCIter() ) {
      const JSON& jsonLink = *j;

      state.links.add();
      State::Link& link = state.links.last();

      link.condition = luaNirvana.compile( jsonLink["if"].get( "" ) );
      link.target    = findState( jsonLink["to"].get( "" ) );
    }
  }
}

Automaton::State* Automaton::update( State* state ) const
{
  return state;
}

}
