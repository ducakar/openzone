/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/modules/QuestModule.hpp
 */

#pragma once

#include "client/Module.hpp"

#include "client/modules/QuestFrame.hpp"

namespace oz
{
namespace client
{

struct Quest
{
  enum State
  {
    PENDING    = 0,
    SUCCESSFUL = 1,
    FAILED     = 2
  };

  String title;
  String description;
  Point3 place;
  int    state;

  Quest();
  explicit Quest( const char* title, const char* description, const Point3& place, int state );
};

class QuestModule : public Module
{
  public:

    Vector<Quest> quests;

    ui::QuestFrame* questFrame;

    virtual void read( InputStream* istream );
    virtual void write( BufferStream* ostream ) const;

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
