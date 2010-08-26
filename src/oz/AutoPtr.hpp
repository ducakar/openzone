/*
 *  AutoPtr.hpp
 *
 *  Auto pointer.
 *  Automatically destructs and deallocates resource it is pointing at. Cannot be copied.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

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

      // no copying
      AutoPtr( const AutoPtr& );
      AutoPtr& operator = ( const AutoPtr& );

    public:

      AutoPtr( Type* pointer_ ) : pointer( pointer_ )
      {}

      AutoPtr( AutoPtr&& p ) : pointer( p.pointer )
      {}

      ~AutoPtr()
      {
        if( pointer != null ) {
          delete pointer;
        }
      }

      AutoPtr& operator = ( AutoPtr&& p )
      {
        pointer = p.pointer;
        p.pointer = null;

        return *this;
      }

      AutoPtr& operator = ( const Type* pointer_ )
      {
        assert( pointer_ == null );

        pointer = pointer_;
        return *this;
      }

      const Type& operator * () const
      {
        return *pointer;
      }

      Type& operator * ()
      {
        return *pointer;
      }

      operator const Type* () const
      {
        return pointer;
      }

      operator Type* () const
      {
        return pointer;
      }

      const Type* operator -> () const
      {
        return pointer;
      }

      Type* operator -> ()
      {
        return pointer;
      }

      const Type& operator[] ( int index ) const
      {
        return pointer[index];
      }

      Type& operator[] ( int index )
      {
        return pointer[index];
      }

      Type* operator + ( int offset ) const
      {
        return pointer + offset;
      }

      Type* operator - ( int offset ) const
      {
        return pointer - offset;
      }

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
