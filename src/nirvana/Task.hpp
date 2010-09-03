/*
 *  Task.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "nirvana/common.hpp"

namespace oz
{
namespace nirvana
{

  class Mind;

  class Task
  {
    friend class DList<Task>;
    friend class Pool<Task>;

    public:

      static const int ACTIVE_BIT = 0x00000001;

      typedef Task* ( * CreateFunc )( const Task* parent );
      typedef Task* ( * ReadFunc )( InputStream* istream, const Task* parent );

      Task*       prev[1];
      Task*       next[1];

      int         flags;

    private:

      Mind*       mind;
      Task*       parent;
      DList<Task> children;

    public:

      static Task* create( const Task* parent );
      static Task* read( InputStream* istream, const Task* parent );

      explicit Task( Mind* mind_, Task* parent_ ) : flags( 0 ), mind( mind_ ), parent( parent_ ) {}
      virtual ~Task() {}

      virtual const char* type() const = 0;

      void addChild( Task* child )
      {
        children.pushLast( child );
      }

      void update()
      {
        if( onUpdate() ) {
          while( children.first() != null && ( ~children.first()->flags & ACTIVE_BIT ) ) {
            delete children.popFirst();
          }
          if( children.first() != null ) {
            children.first()->update();
          }
        }
      }

    protected:

      // return true if children should be updated too
      virtual bool onUpdate() = 0;

    public:

      virtual void write( OutputStream* ostream ) const;

  };

}
}
