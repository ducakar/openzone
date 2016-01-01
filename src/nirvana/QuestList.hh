/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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
 * @file nirvana/QuestList.hh
 */

#pragma once

#include <nirvana/common.hh>

namespace oz
{

struct Quest
{
  enum State
  {
    NONE,
    PENDING,
    SUCCESSFUL,
    FAILED
  };

  String title;
  String description;
  Point  place;
  State  state;
};

class QuestList
{
public:

  List<Quest> quests;
  int         activeQuest;

public:

  QuestList();

  void add(const char* title, const char* description, const Point& place, Quest::State state);
  void remove(int index);

  void read(Stream* is);
  void write(Stream* os) const;

  void load();
  void unload();

};

extern QuestList questList;

}
