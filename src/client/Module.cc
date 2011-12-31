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
 * @file client/Module.cc
 */

#include "stable.hh"

#include "client/Module.hh"

#include "client/modules/ProfileModule.hh"
#include "client/modules/FloraModule.hh"
#include "client/modules/MusicPlayerModule.hh"
#include "client/modules/GalileoModule.hh"
#include "client/modules/QuestModule.hh"

#define OZ_REGISTER_MODULE( module ) \
  list->add( &module##Module )

namespace oz
{
namespace client
{

void Module::listModules( Vector<Module*>* list )
{
  OZ_REGISTER_MODULE( profile );
  OZ_REGISTER_MODULE( flora );
  OZ_REGISTER_MODULE( musicPlayer );
  OZ_REGISTER_MODULE( galileo );
  OZ_REGISTER_MODULE( quest );
}

}
}
