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

#define OZ_VBO_OFFSET( index, Type ) \
  reinterpret_cast<const void*>( ( index ) * sizeof( Type ) )

#define OZ_VBO_OFFSETOF( index, VertexStruct, member ) \
  reinterpret_cast<const void*> \
  ( ( index ) * sizeof( VertexStruct ) + offsetof( VertexStruct, member ) )
