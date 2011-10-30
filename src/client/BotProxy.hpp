/*
 *  BotProxy.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/Proxy.hpp"

namespace oz
{
namespace client
{

class BotProxy : public Proxy
{
  private:

    // leave this much space between obstacle and camera, if camera is brought closer to the eyes
    // because of an obstacle
    static const float THIRD_PERSON_CLIP_DIST;
    static const float BOB_SUPPRESSION_COEF;

    // how far behind the eyes the camera should be
    float externalDistFactor;

    float bobPhi;
    float bobTheta;
    float bobBias;

    bool  isExternal;
    bool  isFreelook;

  public:

    virtual void begin();
    virtual void update();
    virtual void prepare();
    virtual void reset();

    virtual void read( InputStream* istream );
    virtual void write( OutputStream* ostream ) const;

    void init();

};

}
}
