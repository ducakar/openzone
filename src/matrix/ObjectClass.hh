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
 * @file matrix/ObjectClass.hh
 */

#pragma once

#include <matrix/common.hh>

#define OZ_CLASS_ATTRIB(attribBit, varName, defValue) \
  attributes |= attribBit & -config[varName].get(defValue)

#define OZ_CLASS_FLAG(flagBit, varName, defValue) \
  flags |= flagBit & -config[varName].get(defValue)

#define OZ_CLASS_STATE(stateBit, varName, defValue) \
  state |= stateBit & -config[varName].get(defValue)

namespace oz
{

class Object;
class FragPool;

class ObjectClass
{
public:

  static constexpr int MAX_SOUNDS       = 32;

  static constexpr int NIGHT_VISION_BIT = 0x01;
  static constexpr int BINOCULARS_BIT   = 0x02;
  static constexpr int GALILEO_BIT      = 0x04;
  static constexpr int MUSIC_PLAYER_BIT = 0x08;
  static constexpr int SUICIDE_BIT      = 0x10;

  using CreateFunc = ObjectClass* ();

  String                   name;
  String                   title;
  String                   description;

  Vec3                     dim;
  int                      flags;
  float                    life;
  float                    resistance;

  int                      attributes;
  int                      key;
  int                      cost;

  int                      nItems;
  List<const ObjectClass*> defaultItems;

  int                      nFrags;
  const FragPool*          fragPool;

  int                      deviceType;

  int                      imagoType;
  int                      imagoModel;

  int                      audioType;
  int                      audioSounds[MAX_SOUNDS];

  String                   onDestroy;
  String                   onUse;
  String                   onUpdate;
  String                   getStatus;

public:

  ObjectClass() = default;
  virtual ~ObjectClass();

  OZ_NO_COPY(ObjectClass)
  OZ_NO_MOVE(ObjectClass)

  static ObjectClass* createClass();

  virtual void init(const Json& config, const char* name_);

  virtual Object* create(int index, const Point& pos, Heading heading) const;
  virtual Object* create(int index, const Json& json) const;
  virtual Object* create(Stream* is) const;

};

}
