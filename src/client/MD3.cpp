/*
 *  MD3.cpp
 *
 *  http://www.wikipedia.org/MD3_(file_format)
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/MD3.hpp"

#include "client/Context.hpp"
#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

MD3::MD3( int id_ ) : id( id_ ), isLoaded( false )
{}

MD3::~MD3()
{
  OZ_GL_CHECK_ERROR();
}

void MD3::load()
{}

void MD3::drawFrame( int ) const
{}

}
}
