/*
 *  QuestModule.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Module.hpp"
#include "client/Lua.hpp"

namespace oz
{
namespace client
{

  struct Quest
  {
    enum State
    {
      PENDING,
      SUCCESSFUL,
      FAILED
    };

    String title;
    String description;
    int    state;

    explicit Quest( const char* title, const char* description, int state );
  };

  class QuestModule : public Module
  {
    public:

      HashString<Quest> quests;

      virtual void read( InputStream* istream );
      virtual void write( OutputStream* ostream ) const;

      virtual void unload();

      virtual void init();

    private:

      OZ_LUA_API( ozQuestAdd );
      OZ_LUA_API( ozQuestEnd );

  };

  extern QuestModule questModule;

}
}
