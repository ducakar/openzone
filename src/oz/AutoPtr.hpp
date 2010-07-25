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

  template <typename Type>
  class AutoPtr
  {
    private:

      Type* pointer;

    public:

      AutoPtr( Type* pointer_ ) : pointer( pointer_ )
      {}

      ~AutoPtr()
      {
        if( pointer != null ) {
          delete pointer;
        }
      }

      AutoPtr& operator = ( const Type* pointer_ )
      {
        assert( pointer_ == null );

        pointer = pointer_;
        return *this;
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

  };

}
