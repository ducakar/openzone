/*
 *  common.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#define GL_GLEXT_PROTOTYPES

#define OZ_VBO_OFFSET( VertexStruct, member ) \
  reinterpret_cast<const void*>( offsetof( VertexStruct, member ) )
