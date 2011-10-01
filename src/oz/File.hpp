/*
 *  File.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "String.hpp"
#include "Buffer.hpp"

namespace oz
{

  class File
  {
    public:

      enum Type
      {
        NONE,
        REGULAR,
        DIRECTORY,
        OTHER,
        MISSING
      };

    private:

      String filePath;
      Type   type;

    public:

      File()
      {}

      explicit File( const char* path );

      void setPath( const char* path );
      Type getType();

      const char* path() const;
      const char* name() const;
      const char* extension() const;
      String baseName() const;

      bool hasExtension( const char* ext ) const;

      // directory-specific
      static bool mkdir( const char* path, uint mode = 0755 );
      bool ls( DArray<File>* fileList );

  };

}
