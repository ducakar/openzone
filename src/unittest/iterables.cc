/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file unittest/iterables.cc
 */

#include "unittest/unittest.hh"

using namespace oz;

void test_iterables()
{
  Log::out << "+ iterables\n";

  Chain<Foo>::CIterator      icl;
  Chain<Foo>::Iterator       il;
  DChain<Foo>::CIterator     icdl;
  DChain<Foo>::Iterator      idl;
  Array<Foo, 1>::CIterator   ica;
  Array<Foo, 1>::Iterator    ia;
  DArray<Foo>::CIterator     icda;
  DArray<Foo>::Iterator      ida;
  List<Foo>::CIterator       icv;
  List<Foo>::Iterator        iv;
  SList<Foo, 1>::CIterator   icsv;
  SList<Foo, 1>::Iterator    isv;
  Map<Foo>::CIterator        icm;
  Map<Foo>::Iterator         im;
  HashIndex<Foo>::CIterator  ichi;
  HashIndex<Foo>::Iterator   ihi;
  HashString<Foo>::CIterator ichs;
  HashString<Foo>::Iterator  ihs;

  List<Foo*>::Iterator       invalid;

  DChain<Foo> l;
  List<Foo> v;

  l.add( new Foo( 2 ) );
  l.add( new Foo( 3 ) );
  l.add( new Foo( 2 ) );
  l.add( new Foo( 1 ) );

  v.add( 0 );
  v.add( 0 );
  v.add( 0 );
  v.add( 0 );

  iMove( v.iter(), l.iter() );
  iMove( iter( l ), l.iter() );
  iMove( invalid, invalid );
  foreach( i, l.citer() ) {
    OZ_CHECK( *i == -1 );
  }
  OZ_CHECK_CONTENTS( v, 1, 2, 3, 2 );
  OZ_CHECK( !iEquals( l.citer(), v.citer() ) );
  OZ_CHECK( iEquals( citer( l ), citer( l ) ) );

  iSet( l.iter(), 0 );
  iSet( invalid, static_cast<Foo*>( null ) );
  foreach( i, l.citer() ) {
    OZ_CHECK( *i == 0 );
  }

  iCopy( l.iter(), v.citer() );
  iCopy( invalid, invalid );
  OZ_CHECK( iEquals( l.citer(), v.citer() ) );

  iCopy( l.iter(), l.citer() );
  OZ_CHECK( iEquals( citer( l ), v.citer() ) );

  v.add();
  OZ_CHECK( !iEquals( l.citer(), citer( v ) ) );

  v.remove();
  OZ_CHECK( iEquals( citer( l ), citer( v ) ) );

  OZ_CHECK( !iContains( l.citer(), 0 ) );
  OZ_CHECK( !iContains( v.citer(), 0 ) );
  OZ_CHECK( iContains( l.citer(), 1 ) );
  OZ_CHECK( iContains( v.citer(), 1 ) );
  OZ_CHECK( iContains( l.citer(), 2 ) );
  OZ_CHECK( iContains( v.citer(), 2 ) );
  OZ_CHECK( iContains( l.citer(), 3 ) );
  OZ_CHECK( iContains( v.citer(), 3 ) );
  OZ_CHECK( !iContains( invalid, static_cast<Foo*>( null ) ) );

  OZ_CHECK( !iFind( l.citer(), 0 ).isValid() );
  OZ_CHECK( !iFind( v.citer(), 0 ).isValid() );
  OZ_CHECK( !iFind( invalid, static_cast<Foo*>( null ) ).isValid() );

  auto li = l.citer();
  auto vi = v.citer();
  OZ_CHECK( iFind( l.citer(), 1 ) == li );
  OZ_CHECK( iFind( v.citer(), 1 ) == vi );

  ++li;
  ++vi;
  OZ_CHECK( iFind( l.citer(), 2 ) == li );
  OZ_CHECK( iFind( v.citer(), 2 ) == vi );

  ++li;
  ++vi;
  OZ_CHECK( iFind( l.citer(), 3 ) == li );
  OZ_CHECK( iFind( v.citer(), 3 ) == vi );

  OZ_CHECK( !iFindLast( l.citer(), 0 ).isValid() );
  OZ_CHECK( !iFindLast( v.citer(), 0 ).isValid() );
  OZ_CHECK( !iFindLast( invalid, static_cast<Foo*>( null ) ).isValid() );

  li = l.citer();
  vi = v.citer();
  OZ_CHECK( iFindLast( l.citer(), 1 ) == li );
  OZ_CHECK( iFindLast( v.citer(), 1 ) == vi );

  ++li;
  ++vi;
  ++li;
  ++vi;
  OZ_CHECK( iFindLast( l.citer(), 3 ) == li );
  OZ_CHECK( iFindLast( v.citer(), 3 ) == vi );

  ++li;
  ++vi;
  OZ_CHECK( iFindLast( l.citer(), 2 ) == li );
  OZ_CHECK( iFindLast( v.citer(), 2 ) == vi );

  OZ_CHECK( iIndex( l.citer(), 0 ) == -1 );
  OZ_CHECK( iIndex( v.citer(), 0 ) == -1 );
  OZ_CHECK( iIndex( l.citer(), 1 ) == 0 );
  OZ_CHECK( iIndex( v.citer(), 1 ) == 0 );
  OZ_CHECK( iIndex( l.citer(), 2 ) == 1 );
  OZ_CHECK( iIndex( v.citer(), 2 ) == 1 );
  OZ_CHECK( iIndex( l.citer(), 3 ) == 2 );
  OZ_CHECK( iIndex( v.citer(), 3 ) == 2 );
  OZ_CHECK( iIndex( invalid, static_cast<Foo*>( null ) ) == -1 );

  OZ_CHECK( iLastIndex( l.citer(), 0 ) == -1 );
  OZ_CHECK( iLastIndex( v.citer(), 0 ) == -1 );
  OZ_CHECK( iLastIndex( l.citer(), 1 ) == 0 );
  OZ_CHECK( iLastIndex( v.citer(), 1 ) == 0 );
  OZ_CHECK( iLastIndex( l.citer(), 2 ) == 3 );
  OZ_CHECK( iLastIndex( v.citer(), 2 ) == 3 );
  OZ_CHECK( iLastIndex( l.citer(), 3 ) == 2 );
  OZ_CHECK( iLastIndex( v.citer(), 3 ) == 2 );
  OZ_CHECK( iLastIndex( invalid, static_cast<Foo*>( null ) ) == -1 );

  List<Foo*> pv;
  pv.add( new Foo( 1 ) );
  pv.add( new Foo( 2 ) );

  iFree( pv.iter() );
  iFree( invalid );
  OZ_CHECK_CONTENTS( pv, null, null );

  l.free();

  OZ_CHECK( !iEquals( l.citer(), v.citer() ) );
  OZ_CHECK( !iEquals( v.citer(), l.citer() ) );

  v.clear();
  v.dealloc();

  iCopy( l.iter(), v.citer() );
  iMove( l.iter(), v.iter() );
  iSet( l.iter(), 0 );

  OZ_CHECK( iEquals( l.citer(), v.citer() ) );
  OZ_CHECK( !iContains( l.citer(), 0 ) );
  OZ_CHECK( !iFind( l.citer(), 0 ).isValid() );
  OZ_CHECK( !iFindLast( l.citer(), 0 ).isValid() );
  OZ_CHECK( iIndex( l.citer(), 0 ) == -1 );
  OZ_CHECK( iLastIndex( l.citer(), 0 ) == -1 );

  iFree( pv.iter() );
}