/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file tests/test.cc
 */

#include <ozCore/ozCore.hh>

using namespace oz;

class Resource
{
  public:

    mutable volatile int refCount;
    mutable SpinLock     refLock;

    OZ_ALWAYS_INLINE
    explicit Resource() :
      refCount( 0 )
    {}

    OZ_ALWAYS_INLINE
    ~Resource()
    {
      hard_assert( refCount == 0 );
    }

    OZ_ALWAYS_INLINE
    void refIncrement() const
    {
      refLock.lock();
      ++refCount;
      refLock.unlock();
    }

    OZ_ALWAYS_INLINE
    void refDecrement() const
    {
      refLock.lock();
      --refCount;
      refLock.unlock();
    }

};

template <class ResourceType>
class Ref
{
  private:

    ResourceType* resource;

  public:

    OZ_ALWAYS_INLINE
    Ref() :
      resource( nullptr )
    {}

    Ref( ResourceType* r ) :
      resource( r )
    {
      if( resource != nullptr ) {
        resource->refIncrement();
      }
    }

    ~Ref()
    {
      release();
    }

    Ref( const Ref& ref ) :
      resource( ref.resource )
    {
      if( resource != nullptr ) {
        resource->refIncrement();
      }
    }

    Ref( Ref&& ref ) :
      resource( ref.resource )
    {
      ref.resource = nullptr;
    }

    Ref& operator = ( const Ref& ref )
    {
      if( &ref == this ) {
        return *this;
      }

      release();

      resource = ref.resource;

      if( resource != nullptr ) {
        resource->refIncrement();
      }

      return *this;
    }

    Ref& operator = ( Ref&& ref )
    {
      if( &ref == this ) {
        return *this;
      }

      release();

      resource = ref.resource;
      ref.resource = nullptr;

      return *this;
    }

    OZ_ALWAYS_INLINE
    operator const ResourceType* () const
    {
      return resource->isLoaded ? resource : nullptr;
    }

    OZ_ALWAYS_INLINE
    operator ResourceType* ()
    {
      return resource->isLoaded ? resource : nullptr;
    }

    OZ_ALWAYS_INLINE
    const ResourceType* operator -> () const
    {
      return resource->isLoaded ? resource : nullptr;
    }

    OZ_ALWAYS_INLINE
    ResourceType* operator -> ()
    {
      return resource->isLoaded ? resource : nullptr;
    }

    void release()
    {
      if( resource != nullptr ) {
        hard_assert( resource->refCount > 0 );

        ResourceType* r = resource;

        resource->refDecrement();
        resource = nullptr;

        if( r->refCount == 0 ) {
          delete r;
        }
      }
    }

};

struct Foo : Resource
{
  ~Foo()
  {
    Log() << "~Foo()\n";
  }
};

int main()
{
  System::init();

  Ref<Foo> foo = new Foo();
  Ref<Foo> bar = foo;

  foo = new Foo();
  Log() << "a\n";
  foo = bar;
  Log() << "a\n";
  return 0;
}
