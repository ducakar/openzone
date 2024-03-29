/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
 * @file nirvana/Task.hh
 */

#pragma once

#include <nirvana/common.hh>

namespace oz
{

class Mind;

class Task : public DChainNode<Task>
{
public:

  static constexpr int ACTIVE_BIT = 0x00000001;

  using CreateFunc = Task* (const Task* parent);
  using ReadFunc = Task* (Stream* is, const Task* parent);

  int          flags = 0;

private:

  Mind*        mind;
  Task*        parent;
  DChain<Task> children;

public:

  static Task* create(const Task* parent);
  static Task* read(Stream* is, const Task* parent);

  explicit Task(Mind* mind_, Task* parent_)
    : mind(mind_), parent(parent_)
  {}

  virtual ~Task();

  OZ_NO_COPY(Task)
  OZ_NO_MOVE(Task)

  virtual const char* type() const = 0;

  void addChild(Task* child)
  {
    children.pushLast(child);
  }

  void update()
  {
    if (onUpdate()) {
      while (children.first() != nullptr && !(children.first()->flags & ACTIVE_BIT)) {
        delete children.popFirst();
      }
      if (children.first() != nullptr) {
        children.first()->update();
      }
    }
  }

protected:

  // return true if children should be updated too
  virtual bool onUpdate() = 0;

public:

  virtual void write(Stream* os) const;

};

}
