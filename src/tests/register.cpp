/*
 *  register.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hpp"

#include <iostream>
#include <map>
#include <string>

#define REGISTER( t ) \
  struct Register_##t { \
    Register_##t() { \
      registry[#t] = &t::build; \
    } \
  }; \
  Register_##t register_##t;

#include <SDL_main.h>

using namespace std;

class Object;

struct Object
{
  float x, y, z;

  virtual string name() = 0;

  virtual ~Object() {}
};

struct Human : Object
{
  static Object* build();

  string name()
  {
    return S( Human );
  }

  virtual ~Human() {}
};

struct Vehicle : Object
{
  static Object* build();

  string name()
  {
    return S( Vehicle );
  }

  virtual ~Vehicle() {}
};

map<string, Object* (*)()> registry;

REGISTER( Human )

Object* Human::build()
{
  return new Human;
}

REGISTER( Vehicle )

Object* Vehicle::build()
{
  return new Vehicle;
}

int main( int, char** )
{
  Object* o = registry["Vehicle"]();
  cout << o->name() << endl;
  return 0;
}
