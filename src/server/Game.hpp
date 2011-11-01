/**
 * Game.hpp
 *
 * [description]
 *
 * Copyright (C) 2002-2011  Davorin Učakar.
 * This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

namespace oz
{
namespace server
{

 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
class Game
{
  public:

    bool init();
    void start();

    void update();

    void stop();
    void free();

};

extern Game game;

}
}
