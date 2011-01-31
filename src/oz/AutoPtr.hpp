/*
 *  AutoPtr.hpp
 *
 *  Auto pointer.
 *  Automatically destructs and deallocates resource it is pointing at. Cannot be copied.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "common.hpp"

namespace oz
{

  /**
   * Auto pointer (similar to std::auto_ptr).
   */
  template <typename Type>
  class AutoPtr
  {
    private:

      Type* pointer;

    public:

      OZ_ALWAYS_INLINE
      explicit AutoPtr() : pointer( null )
      {}

      OZ_ALWAYS_INLINE
      AutoPtr( Type* pointer_ ) : pointer( pointer_ )
      {}

      OZ_ALWAYS_INLINE
      AutoPtr( AutoPtr& p ) : pointer( p.pointer )
      {
        p.pointer = null;
      }

      ~AutoPtr()
      {
        if( pointer != null ) {
          delete pointer;
        }
      }


      OZ_ALWAYS_INLINE
      AutoPtr& operator = ( AutoPtr& p )
      {
        assert( pointer == null );

        pointer = p.pointer;
        p.pointer = null;

        return *this;
      }

      OZ_ALWAYS_INLINE
      AutoPtr& operator = ( const Type* pointer_ )
      {
        assert( pointer_ == null );

        pointer = pointer_;
        return *this;
      }

      OZ_ALWAYS_INLINE
      const Type& operator * () const
      {
        return *pointer;
      }

      OZ_ALWAYS_INLINE
      Type& operator * ()
      {
        return *pointer;
      }

      OZ_ALWAYS_INLINE
      operator const Type* () const
      {
        return pointer;
      }

      OZ_ALWAYS_INLINE
      operator Type* () const
      {
        return pointer;
      }

      OZ_ALWAYS_INLINE
      const Type* operator -> () const
      {
        return pointer;
      }

      OZ_ALWAYS_INLINE
      Type* operator -> ()
      {
        return pointer;
      }

      OZ_ALWAYS_INLINE
      const Type& operator[] ( int index ) const
      {
        return pointer[index];
      }

      OZ_ALWAYS_INLINE
      Type& operator[] ( int index )
      {
        return pointer[index];
      }

      OZ_ALWAYS_INLINE
      Type* operator + ( int offset ) const
      {
        return pointer + offset;
      }

      OZ_ALWAYS_INLINE
      Type* operator - ( int offset ) const
      {
        return pointer - offset;
      }

      OZ_ALWAYS_INLINE
      void clear()
      {
        pointer = null;
      }

      void free()
      {
        delete pointer;
        pointer = null;
      }

  };

}
