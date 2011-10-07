/*
 *  QuestFrame.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Frame.hpp"
#include "client/ui/Button.hpp"
#include "client/ui/Text.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class GalileoFrame;

  class QuestFrame : public Frame
  {
    friend class GalileoFrame;

    private:

      static const String statusMessages[];

      Text description;
      int  lastState;
      int  currentQuest;
      int  contentHeight;
      bool isOpened;

      void updateTask();

      static void open( Button* sender );
      static void next( Button* sender );
      static void prev( Button* sender );

    protected:

      virtual void onDraw();

    public:

      QuestFrame();

  };

}
}
}
