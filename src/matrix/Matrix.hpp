/*
 *  Matrix.hpp
 *
 *  World model
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/common.hpp"

namespace oz
{
namespace matrix
{

class Matrix
{
  private:

    int maxStructs;
    int maxEvents;
    int maxObjects;
    int maxDynamics;
    int maxWeapons;
    int maxBots;
    int maxVehicles;
    int maxParts;

  public:

    static const float MAX_VELOCITY2;

    void update();

    void read( InputStream* istream );
    void write( OutputStream* ostream ) const;

    void load();
    void unload();

    void init();
    void free();

};

extern Matrix matrix;

}
}
