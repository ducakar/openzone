/*
 *  register.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

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

using namespace std;

class Object;

struct Object
{
  float x, y, z;

  virtual string name() = 0;
};

struct Human : Object
{
  static Object *build();

  string name()
  {
    return $( Human );
  }
};

struct Vehicle : Object
{
  static Object *build();

  string name()
  {
    return $( Vehicle );
  }
};

map<string, Object *(*)()> registry;

REGISTER( Human );

Object *Human::build()
{
  return new Human;
}

REGISTER( Vehicle );

Object *Vehicle::build()
{
  return new Vehicle;
}

int main()
{
  Object *o = registry["Vehicle"]();
  cout << o->name() << endl;
  return 0;
}
