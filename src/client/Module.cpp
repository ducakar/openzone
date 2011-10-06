/*
 *  Module.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Module.hpp"

#include "client/modules/ProfileModule.hpp"
#include "client/modules/FloraModule.hpp"
#include "client/modules/MusicPlayerModule.hpp"
#include "client/modules/GalileoModule.hpp"
#include "client/modules/QuestModule.hpp"

#define OZ_REGISTER_MODULE( module ) \
  list->add( &module##Module )

namespace oz
{
namespace client
{

  void Module::listModules( Vector<Module*>* list )
  {
#ifndef OZ_TOOLS
    OZ_REGISTER_MODULE( profile );
    OZ_REGISTER_MODULE( flora );
    OZ_REGISTER_MODULE( musicPlayer );
    OZ_REGISTER_MODULE( galileo );
    OZ_REGISTER_MODULE( quest );
#else
    OZ_REGISTER_MODULE( galileo );
#endif
  }

}
}
