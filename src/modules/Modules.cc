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
 * @file modules/Modules.cc
 */

#include <modules/Modules.hh>

#include <modules/flora/FloraModule.hh>

#define OZ_REGISTER_MODULE( moduleName ) \
  modules.add( &moduleName##Module )

namespace oz
{
namespace modules
{

void Modules::update()
{
  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->update();
  }
}

void Modules::read( InputStream* is )
{
  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->read( is );
  }
}

void Modules::write( OutputStream* os ) const
{
  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->write( os );
  }
}

void Modules::load()
{
  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->load();
  }
}

void Modules::unload()
{
  for( int i = modules.length() - 1; i >= 0; --i ) {
    modules[i]->unload();
  }
}

void Modules::registerLua() const
{
  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->registerLua();
  }
}

void Modules::init()
{
  OZ_REGISTER_MODULE( flora );

  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->init();
  }
}

void Modules::destroy()
{
  for( int i = modules.length() - 1; i >= 0; --i ) {
    modules[i]->destroy();
  }

  modules.clear();
  modules.deallocate();
}

Modules modules;

}
}
