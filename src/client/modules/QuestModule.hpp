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

#include "client/modules/QuestFrame.hpp"

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
    Point3 place;
    int    state;

    explicit Quest( const char* title, const char* description, const Point3& place, int state );
  };

  class QuestModule : public Module
  {
    private:

      ui::QuestFrame* questFrame;

    public:

      HashString<Quest> quests;

      virtual void read( InputStream* istream );
      virtual void write( OutputStream* ostream ) const;

      virtual void load();
      virtual void unload();

      virtual void registerLua() const;

      virtual void init();

    private:

      OZ_LUA_API( ozQuestAdd );
      OZ_LUA_API( ozQuestEnd );

  };

  extern QuestModule questModule;

}
}
