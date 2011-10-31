/*
 *  GameStage.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/Stage.hpp"
#include "client/Proxy.hpp"
#include "client/Module.hpp"

namespace oz
{
namespace client
{

class GameStage : public Stage
{
  public:

    enum State
    {
      GAME,
      MENU
    };

    static String AUTOSAVE_FILE;
    static String QUICKSAVE_FILE;

  private:

    SDL_Thread*     auxThread;
    SDL_sem*        mainSemaphore;
    SDL_sem*        auxSemaphore;

    volatile bool   isAlive;

    bool            isLoaded;

    Vector<Module*> modules;

    static int auxMain( void* );

    void run();

    void reload();

  public:

    State  state;
    Proxy* proxy;

    String stateFile;
    String missionFile;

    virtual bool update();
    virtual void present();

    bool read( const char* path );
    void write( const char* path ) const;

    virtual void load();
    virtual void unload();

    virtual void init();
    virtual void free();

};

extern GameStage gameStage;

}
}
