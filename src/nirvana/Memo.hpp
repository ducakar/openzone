/*
 *  Memo.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "nirvana/Device.hpp"

namespace oz
{
namespace nirvana
{

class Memo : public Device
{
  private:

    String text;

  public:

    static Pool<Memo, 1024> pool;

    static Device* create( int id, InputStream* istream );

    explicit Memo( const char* text );

    virtual const char* type() const;

    virtual const char* getMemo() const;

    virtual void write( OutputStream* ostream ) const;

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
