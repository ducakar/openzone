/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "../base/base.h"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <algorithm>

using namespace std;

template <class Type, int STACK_SIZE>
inline void aSort( Type *array, int count )
{
  Type *stack[STACK_SIZE];
  Type **sp = stack;
  Type *first = array;
  Type *last = array + count - 1;

  *( sp++ ) = first;
  *( sp++ ) = last;

  do {
    last = *( --sp );
    first = *( --sp );

    if( first < last ) {
      if( last - first > 10 ) {
        int pivotValue = *last;
        Type *top = first;
        Type *bottom = last - 1;

        do {
          while( top <= bottom && *top <= pivotValue ) {
            top++;
          }
          while( top < bottom && *bottom > pivotValue ) {
            bottom--;
          }
          if( top >= bottom ) {
            break;
          }
          swap( *top, *bottom );
        }
        while( true );

        swap( *top, *last );

        *( sp++ ) = first;
        *( sp++ ) = top - 1;
        *( sp++ ) = top + 1;
        *( sp++ ) = last;
      }
      else {
        // selection sort
        for( Type *i = first; i < last; ) {
          Type *pivot = i;
          Type *min = i;
          i++;

          for( Type *j = i; j <= last; j++ ) {
            if( *j < *min ) {
              min = j;
            }
          }
          swap( *pivot, *min );
        }
      }
    }
  }
  while( sp != stack );
}

template <class Type>
inline void arSort( Type *first, Type *last )
{
  if( first < last ) {
    if( last - first > 1 ) {
      int pivotValue = *last;
      Type *top = first;
      Type *bottom = last - 1;

      do {
        while( top <= bottom && *top <= pivotValue ) {
          top++;
        }
        while( top < bottom && *bottom > pivotValue ) {
          bottom--;
        }
        if( top >= bottom ) {
          break;
        }
        swap( *top, *bottom );
      }
      while( true );

      swap( *top, *last );

      arSort( first, top - 1 );
      arSort( top + 1, last );
    }
    else if( *first > *last ) {
      swap( *first, *last );
    }
  }
}

template <class Type>
static void oaSort( Type *array, int begin, int end )
{
  int first = begin;
  int last = end - 1;

  if( first < last ) {
    if( first + 1 == last ) {
      if( array[first] > array[last] ) {
        swap( array[first], array[last] );
      }
    }
    else {
      int pivotValue = array[last];
      int top = first;
      int bottom = last - 1;

      do {
        while( top <= bottom && array[top] <= pivotValue ) {
          top++;
        }
        while( top < bottom && array[bottom] > pivotValue ) {
          bottom--;
        }
        if( top < bottom ) {
          swap( array[top], array[bottom] );
        }
        else {
          break;
        }
      }
      while( true );

      swap( array[top], array[last] );
      oaSort( array, begin, top );
      oaSort( array, top + 1, end );
    }
  }
}

#define minForQSort 10

template <class T>
void TQuickSortInc( T *a, int num_el )
{
   int s_pos=0;
   T *first = a, *last = a + (num_el - 1);
   T *stack[32*2];

   T temp;

   do
   {
      do
      {
         if (last-first <= minForQSort)
         {

         T *lo = first, *hi = last;
         while(hi > lo)
         {
            T *max = lo;
            for( T *p = lo+1; p <= hi; p++ )
            {
               if(!(p[0] < max[0])) max = p;
            }
            temp = max[0]; max[0] = hi[0];  hi[0] = temp;

            hi--;
         }

         break;
         }



         T *mid = (last - first >> 1) + first;
         temp = first[0]; first[0] = mid[0];  mid[0] = temp;


         T *cfirst = first,
         *clast = last + 1;



         do
         {
            do
            {
               cfirst++;
            }
            while( cfirst <= last && cfirst[0] < first[0] );

            do
            {
               clast--;
            }
            while( clast > first && !(clast[0] < first[0] ));

            if (clast < cfirst) break;
            temp = cfirst[0]; cfirst[0] = clast[0];  clast[0] = temp;
         }
         while(1);

         temp = first[0]; first[0] = clast[0];  clast[0] = temp;



         if( clast-1 - first >= last - cfirst )
         {
            if( first + 1 < clast )
            {
               stack[ s_pos ] = first;
               stack[ s_pos+1 ] = clast - 1;
               s_pos += 2;
            }

            if( cfirst < last )
            {
               first = cfirst;
            }
            else break;
         }
         else
            {
            if( cfirst < last )
            {
               stack[ s_pos ] = cfirst;
               stack[ s_pos+1 ] = last;
               s_pos += 2;
            }

            if( first + 1 < clast )
            {
               last = clast - 1;
            }
            else break;
         }
      }
      while(1);



      if( !s_pos ) break;
      s_pos -=2;  first = stack[ s_pos ];  last = stack[ s_pos+1 ];
   }
   while(1);
}

#define MAX 200
#define TESTS 500

int main( int, char *[] )
{
  int array[MAX];

  srand( 32 );

  long t0 = clock();
  for( int i = 0; i < TESTS; i++ ) {
    for( int j = 0; j < MAX; j++ ) {
      array[j] = rand() % MAX;
    }
    //sort( array, 0, MAX );
    //quickSort( array, MAX );
    //arSort( array, array + MAX - 1 );
    Dark::aSort( array, MAX );
    //TQuickSortInc( array, MAX );
    //aSort<int, 2048>( array, MAX );
  }
  printf( "%d\n", static_cast<int>( clock() - t0 ) / 1000 );
  for( int i = 0; i < MAX; i++ ) {
    printf( "%d ", array[i] );
  }
  return 0;
}
