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
 * @file builder/Compiler.cc
 */

#include <builder/Compiler.hh>

#include <builder/Context.hh>

namespace oz
{
namespace builder
{

static const Vec3 DIRS[] = {
  Vec3( +1.0f, +1.0f, +1.0f ),
  Vec3( -1.0f, +1.0f, +1.0f ),
  Vec3( +1.0f, -1.0f, +1.0f ),
  Vec3( -1.0f, -1.0f, +1.0f ),
  Vec3( +1.0f, +1.0f, -1.0f ),
  Vec3( -1.0f, +1.0f, -1.0f ),
  Vec3( +1.0f, -1.0f, -1.0f ),
  Vec3( -1.0f, -1.0f, -1.0f )
};

enum Environment
{
  NONE,
  MODEL,
  MESH,
  POLY
};

struct Vertex
{
  Point    pos;
  TexCoord texCoord;
  Vec3     normal;
  Vec3     tangent;
  Vec3     binormal;
  Vec3     colour;

  OZ_HIDDEN
  bool operator == ( const Vertex& v ) const
  {
    return pos == v.pos && texCoord == v.texCoord && normal == v.normal && tangent == v.tangent &&
           binormal == v.binormal && colour == v.colour;
  }

  OZ_HIDDEN
  void write( OutputStream* ostream ) const;
};

struct Triangle
{
  float depth;
  int   index;

  OZ_HIDDEN
  bool operator < ( const Triangle& t ) const
  {
    return depth < t.depth;
  }
};

struct Mesh
{
  int          flags;
  String       texture;

  int          firstIndex;
  int          nIndices;
  List<ushort> indices;
};

struct Node
{
  static Pool<Node> pool;

  String      name;
  Mat44       transf;
  int         mesh;

  Node*       parent;
  List<Node*> children;

  Node*       prev[1];
  Node*       next[1];

  int         firstChild;
  int         nChildren;

  OZ_HIDDEN
  explicit Node( const char* name_ = "", Node* parent_ = nullptr ) :
    name( name_ ), transf( Mat44::ID ), mesh( -1 ), parent( parent_ )
  {}

  OZ_HIDDEN
  ~Node()
  {
    children.free();
  }

  OZ_STATIC_POOL_ALLOC( pool )
};

Pool<Node>                Node::pool;

static DArray<Point>      positions;
static DArray<Vec3>       normals;
static List<Vertex>       vertices;
static List<Mesh>         meshes;
static List<Node*>        nodes;

static Bounds             bounds;

static Vertex             vert;
static Mesh               mesh;
static Node               root;
static Node*              node;

static Environment        environment;
static int                caps;
static String             shaderName;
static int                nFrames;
static int                nFramePositions;
static Compiler::PolyMode mode;
static int                vertNum;
static List<ushort>       polyIndices;

OZ_HIDDEN
void Vertex::write( OutputStream* ostream ) const
{
  ostream->writePoint( pos );
  ostream->writeFloat( texCoord.u );
  ostream->writeFloat( texCoord.v );
  ostream->writeVec3( normal );
}

static void calculateBounds( const Node* node, const Mat44& transf )
{
  Mat44 nodeTransf = transf * node->transf;

  if( node->mesh >= 0 ) {
    const Mesh& mesh = meshes[node->mesh];

    foreach( index, mesh.indices.citer() ) {
      Point pos = vertices[*index].pos;

      if( nFrames != 0 ) {
        pos = positions[ int( pos.x ) ];
      }

      pos = nodeTransf * pos;

      bounds.mins = min( bounds.mins, pos );
      bounds.maxs = max( bounds.maxs, pos );
    }
  }

  foreach( child, node->children.citer() ) {
    calculateBounds( *child, nodeTransf );
  }
}

static void storeNode( Node* node, int depth )
{
  if( depth != 0 ) {
    foreach( child, node->children.citer() ) {
      storeNode( *child, depth - 1 );
    }
  }
  else {
    node->firstChild = nodes.length();
    node->nChildren  = node->children.length();

    nodes.addAll( node->children.begin(), node->children.length() );
  }
}

void Compiler::enable( Capability cap )
{
  caps |= cap;
}

void Compiler::disable( Capability cap )
{
  caps &= ~cap;
}

void Compiler::beginModel()
{
  hard_assert( environment == NONE );

  positions.clear();
  normals.clear();
  vertices.clear();
  meshes.clear();
  nodes.clear();
  root.children.free();

  bounds.mins     = Point( +Math::INF, +Math::INF, +Math::INF );
  bounds.maxs     = Point( -Math::INF, -Math::INF, -Math::INF );

  mesh.flags      = Model::SOLID_BIT;
  mesh.texture    = "";

  root            = Node( nullptr );
  node            = &root;

  environment     = MODEL;
  caps            = 0;
  mode            = TRIANGLES;
  shaderName      = "mesh";
  vertNum         = 0;
  nFrames         = 0;
  nFramePositions = 0;
}

void Compiler::endModel()
{
  hard_assert( environment == MODEL );
  environment = NONE;

  calculateBounds( &root, Mat44::ID );
}

void Compiler::shader( const char* shaderName_ )
{
  hard_assert( environment == MODEL );

  shaderName = shaderName_;
}

void Compiler::anim( int nFrames_, int nFramePositions_ )
{
  hard_assert( environment == MODEL );

  if( nFrames <= 1 ) {
    nFrames         = 0;
    nFramePositions = 0;
  }

  nFrames         = nFrames_;
  nFramePositions = nFramePositions_;

  positions.resize( nFrames * nFramePositions );
  normals.resize( nFrames * nFramePositions );
}

void Compiler::animPositions( const float* positions_ )
{
  hard_assert( environment == MODEL );
  hard_assert( nFrames != 0 );

  for( int i = 0; i < positions.length(); ++i ) {
    positions[i].x = *positions_++;
    positions[i].y = *positions_++;
    positions[i].z = *positions_++;
  }
}

void Compiler::animNormals( const float* normals_ )
{
  hard_assert( environment == MODEL );
  hard_assert( nFrames != 0 );

  for( int i = 0; i < normals.length(); ++i ) {
    normals[i].x = *normals_++;
    normals[i].y = *normals_++;
    normals[i].z = *normals_++;
  }
}

void Compiler::beginNode( const char* name )
{
  hard_assert( environment == MODEL );

  Node* newNode = new Node( name, node );

  node->children.add( newNode );
  node = newNode;
}

void Compiler::endNode()
{
  hard_assert( environment == MODEL && node != &root );

  node = node->parent;
}

void Compiler::transform( const Mat44& t )
{
  hard_assert( environment == MODEL && node != &root );

  node->transf = t;
}

void Compiler::bindMesh( int id )
{
  hard_assert( environment == MODEL && node != &root );

  node->mesh = id;
}

void Compiler::beginMesh()
{
  hard_assert( environment == MODEL );
  environment = MESH;

  mesh.flags   = Model::SOLID_BIT;
  mesh.texture = "";
}

int Compiler::endMesh()
{
  hard_assert( environment == MESH );
  environment = MODEL;

  int index = meshes.length();

  meshes.add( mesh );
  mesh.indices.clear();

  return index;
}

void Compiler::begin( Compiler::PolyMode mode_ )
{
  hard_assert( environment == MESH );
  environment = POLY;

  vert.pos        = Point::ORIGIN;
  vert.texCoord   = TexCoord( 0.0f, 0.0f );
  vert.normal     = Vec3::ZERO;
  vert.tangent    = Vec3::ZERO;
  vert.binormal   = Vec3::ZERO;
  vert.colour     = Vec3::ZERO;

  mode            = mode_;
  vertNum         = 0;
  polyIndices.clear();
}

void Compiler::end()
{
  hard_assert( environment == POLY );
  environment = MESH;

  if( caps & CLOCKWISE ) {
    polyIndices.reverse();
  }

  switch( mode ) {
    case TRIANGLES: {
      hard_assert( vertNum >= 3 && vertNum % 3 == 0 );

      mesh.indices.takeAll( polyIndices.begin(), polyIndices.length() );
      break;
    }
    case POLYGON: {
      hard_assert( vertNum >= 3 );

      int last[2] = { 0, 1 };

      for( int i = 0; i < vertNum / 2; ++i ) {
        int j = ( i + 3 ) / 2;

        if( i & 1 ) {
          mesh.indices.add( polyIndices[ last[0] ] );
          mesh.indices.add( polyIndices[ last[1] ] );
          mesh.indices.add( polyIndices[j] );

          last[1] = j;
        }
        else {
          mesh.indices.add( polyIndices[ last[0] ] );
          mesh.indices.add( polyIndices[ last[1] ] );
          mesh.indices.add( polyIndices[vertNum - j] );

          last[0] = vertNum - j;
        }
      }
      break;
    }
  }
}

void Compiler::texture( const char* texture )
{
  hard_assert( environment == MESH );

  mesh.texture = texture;
}

void Compiler::blend( bool doBlend )
{
  hard_assert( environment == MESH );

  mesh.flags = doBlend ? Model::ALPHA_BIT : Model::SOLID_BIT;
}

void Compiler::texCoord( float u, float v )
{
  hard_assert( environment == POLY );

  vert.texCoord[0] = u;
  vert.texCoord[1] = v;
}

void Compiler::texCoord( const float* v )
{
  texCoord( v[0], v[1] );
}

void Compiler::normal( float x, float y, float z )
{
  hard_assert( environment == POLY );

  vert.normal.x = x;
  vert.normal.y = y;
  vert.normal.z = z;
}

void Compiler::normal( const float* v )
{
  normal( v[0], v[1], v[2] );
}

void Compiler::vertex( float x, float y, float z )
{
  hard_assert( environment == POLY );
  hard_assert( nFrames == 0 || ( y == 0.0f && z == 0.0f ) );

  vert.pos.x = x;
  vert.pos.y = y;
  vert.pos.z = z;

  int index;

  if( caps & UNIQUE ) {
    index = vertices.include( vert );

    polyIndices.add( ushort( index ) );
  }
  else {
    index = vertices.length();

    vertices.add( vert );
    polyIndices.add( ushort( index ) );
  }

  ++vertNum;
}

void Compiler::vertex( const float* v )
{
  vertex( v[0], v[1], v[2] );
}

void Compiler::animVertex( int i )
{
  hard_assert( environment == POLY );
  hard_assert( nFrames > 1 && uint( i ) < uint( positions.length() ) );

  mesh.flags |= Model::ANIMATED_BIT;

  vertex( float( i ), 0.0f, 0.0f );
}

void Compiler::writeModel( OutputStream* os, bool globalTextures )
{
  hard_assert( environment == NONE );
  hard_assert( meshes.length() > 0 && vertices.length() > 0 );
  hard_assert( positions.length() == normals.length() );

  Log::print( "Writing mesh ..." );

  List<String>   textures;
  List<Triangle> triangles;
  List<ushort>   indices[8];

  int nIndices = 0;

  for( int i = 0; i < meshes.length(); ++i ) {
    textures.include( meshes[i].texture );

    meshes[i].firstIndex = nIndices;
    meshes[i].nIndices   = meshes[i].indices.length();

    // Sort triangles for every (+/-x, +/-y, +/-z) direction.
    for( int j = 0; j < 8; ++j ) {
      for( int k = 0; k < meshes[i].indices.length(); k += 3 ) {
        Vec3 centre = ( ( vertices[ meshes[i].indices[k + 0] ].pos - Point::ORIGIN ) +
                        ( vertices[ meshes[i].indices[k + 1] ].pos - Point::ORIGIN ) +
                        ( vertices[ meshes[i].indices[k + 2] ].pos - Point::ORIGIN ) ) / 3.0f;

        triangles.add( { centre * DIRS[j], k } );
      }

      triangles.sort();

      for( int k = 0; k < triangles.length(); ++k ) {
        indices[j].add( meshes[i].indices[ triangles[k].index + 0 ] );
        indices[j].add( meshes[i].indices[ triangles[k].index + 1 ] );
        indices[j].add( meshes[i].indices[ triangles[k].index + 2 ] );
      }

      triangles.clear();
    }

    nIndices += meshes[i].nIndices;
  }

  int nNodes = -1;

  for( int i = 0; nNodes != nodes.length(); ++i ) {
    nNodes = nodes.length();

    storeNode( &root, i );
  }

  os->writeVec3( bounds.dim() );

  os->writeString( shaderName );
  os->writeInt( globalTextures ? ~textures.length() : textures.length() );
  os->writeInt( vertices.length() );
  os->writeInt( nIndices );
  os->writeInt( nFrames );
  os->writeInt( nFramePositions );

  os->writeInt( meshes.length() );
  os->writeInt( Node::pool.length() );

  foreach( texture, textures.citer() ) {
    os->writeString( *texture );
  }

  foreach( vertex, vertices.iter() ) {
    if( nFrames != 0 ) {
      vertex->pos = Point( ( vertex->pos[0] + 0.5f ) / float( nFramePositions ), 0.0f, 0.0f );
    }
    vertex->write( os );
  }
  for( int i = 0; i < 8; ++i ) {
    foreach( index, indices[i].citer() ) {
      os->writeUShort( *index );
    }
  }

  if( nFrames != 0 ) {
    hard_assert( positions.length() == nFrames * nFramePositions );
    hard_assert( normals.length() == nFrames * nFramePositions );

    foreach( position, positions.citer() ) {
      os->writePoint( *position );
    }
    foreach( normal, normals.citer() ) {
      os->writeVec3( *normal );
    }
  }

  foreach( mesh, meshes.citer() ) {
    os->writeInt( mesh->flags );
    os->writeInt( textures.index( mesh->texture ) );

    os->writeInt( mesh->nIndices );
    os->writeInt( mesh->firstIndex );
  }

  foreach( i, nodes.citer() ) {
    const Node* node = *i;

    os->writeMat44( node->transf );
    os->writeInt( node->firstChild );
    os->writeInt( node->nChildren );
    os->writeInt( node->mesh );
  }

  Log::printEnd( " OK" );
}

void Compiler::buildModelTextures( const char* destDir )
{
  hard_assert( environment == NONE );

  List<String> textures;

  for( int i = 0; i < meshes.length(); ++i ) {
    textures.include( meshes[i].texture );
  }

  for( int i = 0; i < textures.length(); ++i ) {
    context.buildTexture( textures[i], String( destDir, "/" ) + textures[i].fileName() );
  }
}

void Compiler::init()
{}

void Compiler::destroy()
{
  positions.clear();
  normals.clear();

  vertices.clear();
  vertices.deallocate();

  meshes.clear();
  meshes.deallocate();

  nodes.clear();
  nodes.deallocate();

  mesh.texture = "";
  mesh.indices.clear();
  mesh.indices.deallocate();

  root.children.free();
  Node::pool.free();

  polyIndices.clear();
  polyIndices.deallocate();
}

Compiler compiler;

}
}
