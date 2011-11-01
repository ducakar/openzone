#include <cstdio>

int main( int argc, char** argv )
{
  for( int i = 1; i < argc; ++i ) {
    FILE* file = fopen( argv[i], "rb" );

    if( file == nullptr ) {
      continue;
    }

    int nBlanks = 0;
    char ch;
    do {
      ch = char( fgetc( file ) );

      if( ch == '\n' || ch == EOF ) {
        ++nBlanks;
      }
      else {
        nBlanks = 0;
      }

      if( nBlanks == 3 ) {
        printf( "%s\n", argv[i] );
        break;
      }
    }
    while( ch != EOF );

    fclose( file );
  }
}
