/*
 *  Directory.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "String.hpp"

namespace oz
{

  class Directory
  {
    public:

      class CIterator : public CIteratorBase<const char>
      {
        private:

          typedef CIteratorBase<const char> B;

          void* dir;

        public:

          CIterator();

          explicit CIterator( const Directory& d );

          CIterator& operator ++ ();

          String baseName() const;
          String extension() const;
          bool hasExtension() const;
          bool hasExtension( const char* ext ) const;

      };

    private:

      void*  dir;
      String path;

    public:

      Directory();
      ~Directory();

    private:

      // no copying
      Directory( const Directory& );
      Directory& operator = ( const Directory& );

    public:

      explicit Directory( const char* path );

      CIterator citer() const;

      bool isOpened() const;
      const char* name() const;

      void open( const char* path );
      void close();

  };

}
