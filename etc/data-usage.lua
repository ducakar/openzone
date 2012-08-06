#!/usr/bin/lua

SIZE = 4096
DIM = 16
TERRA_DIM = 8

PTR_SIZE = 8
VEC_SIZE = 12

CELL_SIZE = ( 1 + 1 ) * PTR_SIZE + ( 6 * 2 + 4 )
QUAD_SIZE = 3 * VEC_SIZE

nCells1 = ( math.floor( ( SIZE - 1 ) / DIM ) + 1 )
nQuads1 = ( math.floor( ( SIZE - 1 ) / TERRA_DIM ) + 2 )
nCells2 = nCells1*nCells1
nQuads2 = nQuads1*nQuads1

-- size
cellsSize = nCells2 * CELL_SIZE / ( 1024.0*1024.0 )
quadsSize = nQuads2 * QUAD_SIZE / ( 1024.0*1024.0 )

print( string.format( 'cells %6.2f MiB = %d x %d x %d B', cellsSize, nCells1, nCells1, CELL_SIZE ) )
print( string.format( 'quads %6.2f MiB = %d x %d x %d B', quadsSize, nQuads1, nQuads1, QUAD_SIZE ) )
print( string.format( '    = %6.2f MiB', cellsSize + quadsSize ) )
