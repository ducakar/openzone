/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

using oz::client::Animation;

namespace oz
{
namespace builder
{

struct Node;

enum Environment
{
  NONE,
  MODEL,
  MESH,
  POLY,
  LIGHT,
  ANIMATION,
  CHANNEL
};

struct Vertex
{
  Point    pos;
  TexCoord texCoord;
  Vec3     normal;
  Vec3     tangent;
  Vec3     binormal;
  String   boneName[2];
  int      bone[2];
  float    weight[2];

  bool operator == ( const Vertex& v ) const
  {
    return pos == v.pos && texCoord == v.texCoord && normal == v.normal && tangent == v.tangent &&
           binormal == v.binormal && boneName[0] == v.boneName[0] && boneName[1] == v.boneName[1] &&
           weight[0] == v.weight[0] && weight[1] == v.weight[1];
  }

  void write( OutputStream* os ) const
  {
    os->writePoint( pos );

    os->writeShort( short( Math::lround( texCoord.u * 1024.0f ) ) );
    os->writeShort( short( Math::lround( texCoord.v * 1024.0f ) ) );

    os->writeByte( byte( normal.x * 127.0f ) );
    os->writeByte( byte( normal.y * 127.0f ) );
    os->writeByte( byte( normal.z * 127.0f ) );
    os->writeByte( 0 );

    os->writeByte( byte( tangent.x * 127.0f ) );
    os->writeByte( byte( tangent.y * 127.0f ) );
    os->writeByte( byte( tangent.z * 127.0f ) );
    os->writeByte( 0 );

    os->writeByte( byte( binormal.x * 127.0f ) );
    os->writeByte( byte( binormal.y * 127.0f ) );
    os->writeByte( byte( binormal.z * 127.0f ) );
    os->writeByte( 0 );

    os->writeUByte( ubyte( bone[0] ) );
    os->writeUByte( ubyte( bone[1] ) );
    os->writeUByte( ubyte( weight[0] * 255.0f ) );
    os->writeUByte( ubyte( weight[1] * 255.0f ) );
  }
};

struct Triangle
{
  float depth;
  int   index;

  bool operator < ( const Triangle& t ) const
  {
    return depth < t.depth;
  }
};

struct Mesh
{
  int          flags;
  String       texture;
  float        shininess;

  int          firstIndex;
  int          nIndices;
  List<ushort> indices;
};

struct Light : client::Light
{
  Node* node;
};

struct Node
{
  static Pool<Node> pool;

  Mat4        transf;
  int         firstChild;
  int         nChildren;
  int         mesh;
  String      name;

  bool        includeBounds;

  Node*       parent;
  List<Node*> children;

  Node*       prev[1];
  Node*       next[1];

  explicit Node( const char* name_ = "", Node* parent_ = nullptr ) :
    transf( Mat4::ID ), mesh( -1 ), name( name_ ), includeBounds( true ), parent( parent_ )
  {}

  Node& operator = ( Node&& ) = default;

  ~Node()
  {
    children.free();
  }

  OZ_STATIC_POOL_ALLOC( pool )
};

Pool<Node>                Node::pool;

static List<Point>        positions;
static List<Vec3>         normals;
static List<Vertex>       vertices;
static List<Mesh>         meshes;
static List<Light>        lights;
static List<Node*>        nodes;
static List<Animation>    animations;

static Bounds             bounds;

static Vertex             vert;
static Mesh               mesh;
static Light              light;
static Node               root;
static Node*              node;
static Animation::Channel channel;
static Animation          animation;

static Environment        environment;
static int                caps;
static String             shaderName;
static int                nFrames;
static int                nFramePositions;
static Compiler::PolyMode mode;
static int                vertNum;
static List<ushort>       polyIndices;

static void calculateBounds( const Node* node, const Mat4& parentTransf )
{
  Mat4 transf = parentTransf ^ node->transf;

  if( node->includeBounds && node->mesh >= 0 ) {
    const Mesh& mesh = meshes[node->mesh];

    for( int index : mesh.indices ) {
      Point pos = vertices[index].pos;

      if( nFrames != 0 ) {
        pos = positions[ int( pos.x ) ];
      }

      pos = transf * pos;

      bounds.mins = min( bounds.mins, pos );
      bounds.maxs = max( bounds.maxs, pos );
    }
  }

  for( const Node* child : node->children ) {
    calculateBounds( child, transf );
  }
}

static void storeNode( Node* node, int depth )
{
  if( depth != 0 ) {
    for( Node* child : node->children ) {
      storeNode( child, depth - 1 );
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
  positions.trim();
  normals.clear();
  normals.trim();
  vertices.clear();
  vertices.trim();
  meshes.clear();
  meshes.trim();
  lights.clear();
  lights.trim();
  nodes.clear();
  nodes.trim();
  root.children.free();

  bounds.mins          = Point( +Math::INF, +Math::INF, +Math::INF );
  bounds.maxs          = Point( -Math::INF, -Math::INF, -Math::INF );

  mesh.flags           = Model::SOLID_BIT;
  mesh.texture         = "";

  light.pos            = Point::ORIGIN;
  light.dir            = Vec3::ZERO;
  light.colour         = Vec3::ZERO;
  light.coneCoeff[0]   = 0.0f;
  light.coneCoeff[1]   = 0.0f;
  light.attenuation[0] = 0.0f;
  light.attenuation[1] = 0.0f;
  light.attenuation[2] = 0.0f;
  light.type           = Light::POINT;

  root                 = Node( nullptr );
  node                 = &root;

  environment          = MODEL;
  caps                 = 0;
  mode                 = TRIANGLES;
  shaderName           = "mesh";
  vertNum              = 0;
  nFrames              = 0;
  nFramePositions      = 0;
}

void Compiler::endModel()
{
  hard_assert( environment == MODEL );
  environment = NONE;

  calculateBounds( &root, Mat4::ID );
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

void Compiler::transform( const Mat4& t )
{
  hard_assert( environment == MODEL && node != &root );

  node->transf = t;
}

void Compiler::includeBounds( bool value )
{
  hard_assert( environment == MODEL && node != &root );

  node->includeBounds = value;
}

void Compiler::bindMesh( int id )
{
  hard_assert( environment == MODEL && node != &root );

  node->mesh = id;
}

void Compiler::bindLight( int id )
{
  hard_assert( environment == MODEL && node != &root );

  lights[id].node = node;
}

void Compiler::beginMesh()
{
  hard_assert( environment == MODEL );
  environment = MESH;

  mesh.flags     = Model::SOLID_BIT;
  mesh.texture   = "";
  mesh.shininess = 50.0f;
}

int Compiler::endMesh()
{
  hard_assert( environment == MESH );
  environment = MODEL;

  meshes.add( mesh );
  mesh.indices.clear();

  return meshes.length() - 1;
}

void Compiler::texture( const char* texture )
{
  hard_assert( environment == MESH );

  mesh.texture = texture;
}

void Compiler::shininess( float exponent )
{
  mesh.shininess = exponent;
}

void Compiler::blend( bool doBlend )
{
  hard_assert( environment == MESH );

  mesh.flags = doBlend ? Model::ALPHA_BIT : Model::SOLID_BIT;
}

void Compiler::begin( Compiler::PolyMode mode_ )
{
  hard_assert( environment == MESH );
  environment = POLY;

  vert.pos         = Point::ORIGIN;
  vert.texCoord    = TexCoord( 0.0f, 0.0f );
  vert.normal      = Vec3::ZERO;
  vert.tangent     = Vec3::ZERO;
  vert.binormal    = Vec3::ZERO;
  vert.boneName[0] = "";
  vert.boneName[1] = "";
  vert.bone[0]     = 0;
  vert.bone[1]     = 0;
  vert.weight[0]   = 0.0f;
  vert.weight[1]   = 0.0f;

  mode             = mode_;
  vertNum          = 0;
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
      int top     = vertNum - 1;
      int bottom  = 2;

      for( int i = 0; bottom <= top; ++i ) {
        if( i & 1 ) {
          mesh.indices.add( polyIndices[ last[0] ] );
          mesh.indices.add( polyIndices[ last[1] ] );
          mesh.indices.add( polyIndices[bottom] );

          last[1] = bottom;
          ++bottom;
        }
        else {
          mesh.indices.add( polyIndices[ last[0] ] );
          mesh.indices.add( polyIndices[ last[1] ] );
          mesh.indices.add( polyIndices[top] );

          last[0] = top;
          --top;
        }
      }
      break;
    }
  }
}

void Compiler::boneWeight( int which, const char* name, float weight )
{
  hard_assert( environment == POLY );
  hard_assert( which == 0 || which == 1 );

  vert.boneName[which] = name;
  vert.weight[which]   = weight;
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

  hard_assert( !normals[i] > 0.9f );
  normal( normals[i] );
  vertex( float( i ), 0.0f, 0.0f );
}

void Compiler::beginLight( Light::Type type )
{
  hard_assert( environment == MODEL );
  environment = LIGHT;

  light.type = type;
}

int Compiler::endLight()
{
  hard_assert( environment == LIGHT );
  environment = MODEL;

  lights.add( light );
  return lights.length() - 1;
}

void Compiler::position( float x, float y, float z )
{
  hard_assert( environment == LIGHT );

  light.pos.x = x;
  light.pos.y = y;
  light.pos.z = z;
}

void Compiler::direction( float x, float y, float z )
{
  hard_assert( environment == LIGHT );

  light.dir.x = x;
  light.dir.y = y;
  light.dir.z = z;
}

void Compiler::colour( float r, float g, float b )
{
  hard_assert( environment == LIGHT );

  light.colour.x = r;
  light.colour.y = g;
  light.colour.z = b;
}

void Compiler::attenuation( float constant, float linear, float quadratic )
{
  hard_assert( environment == LIGHT );

  light.attenuation[0] = constant;
  light.attenuation[1] = linear;
  light.attenuation[2] = quadratic;
}

void Compiler::coneAngles( float inner, float outer )
{
  hard_assert( environment == LIGHT );

  light.coneCoeff[0] = Math::tan( inner / 2.0f );
  light.coneCoeff[1] = Math::tan( outer / 2.0f );
}

void Compiler::beginAnimation()
{
  hard_assert( environment == MODEL );
  environment = ANIMATION;

  animation.channels.clear();
}

void Compiler::endAnimation()
{
  hard_assert( environment == ANIMATION );
  environment = MODEL;

  animations.add( static_cast<Animation&&>( animation ) );
}

void Compiler::beginChannel()
{
  hard_assert( environment == ANIMATION );
  environment = CHANNEL;

  channel.positionKeys.clear();
  channel.rotationKeys.clear();
  channel.scalingKeys.clear();
}

void Compiler::endChannel()
{
  hard_assert( environment == CHANNEL );
  environment = ANIMATION;

  animation.channels.add( static_cast<Animation::Channel&&>( channel ) );
}

void Compiler::positionKey( const Point& pos, float time )
{
  hard_assert( environment == CHANNEL );

  channel.positionKeys.add( { pos, time } );
}

void Compiler::rotationKey( const Quat& rot, float time )
{
  hard_assert( environment == CHANNEL );

  channel.rotationKeys.add( { rot, time } );
}

void Compiler::scalingKey( const Vec3& scale, float time )
{
  hard_assert( environment == CHANNEL );

  channel.scalingKeys.add( { scale, time } );
}

void Compiler::writeModel( OutputStream* os, bool globalTextures )
{
  hard_assert( environment == NONE );
  hard_assert( meshes.length() > 0 && vertices.length() > 0 );
  hard_assert( positions.length() == normals.length() );

  Log::print( "Writing mesh ..." );

  List<String> textures;
  List<ushort> indices;

  int nIndices = 0;

  for( int i = 0; i < meshes.length(); ++i ) {
    textures.include( meshes[i].texture );

    meshes[i].firstIndex = nIndices;
    meshes[i].nIndices   = meshes[i].indices.length();

    indices.addAll( meshes[i].indices.begin(), meshes[i].indices.length() );

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
  os->writeInt( lights.length() );
  os->writeInt( Node::pool.length() );
  os->writeInt( animations.length() );

  for( const String& texture : textures ) {
    os->writeString( texture );
  }

  // Generate tangents and binormals.
  for( int i = 0; i < indices.length(); i += 3 ) {
    Vertex* v[3] = {
      &vertices[ indices[i + 0] ],
      &vertices[ indices[i + 1] ],
      &vertices[ indices[i + 2] ]
    };

    // [ t_x b_x ]   [ p_x q_x ]            -1
    // [ t_y b_y ] = [ p_y q_y ] [ p_u q_u ]
    // [ t_z b_z ]   [ p_z q_z ] [ p_v q_v ]
    //
    // (t, b) - tangent and binormal (non-ortonormised)
    // (p, q) - delta points
    for( int j = 0; j < 3; ++j ) {
      Vec3 normal   = v[j]->normal;
      Vec3 tangent  = Vec3::ZERO;
      Vec3 binormal = Vec3::ZERO;

      if( normal.sqN() != 0.0f ) {
        Vertex* a     = v[j];
        Vertex* b     = v[ ( j + 1 ) % 3 ];
        Vertex* c     = v[ ( j + 2 ) % 3 ];

        Vec3  p       = b->pos - a->pos;
        Vec3  q       = c->pos - a->pos;
        float pu      = b->texCoord.u - a->texCoord.u;
        float pv      = b->texCoord.v - a->texCoord.v;
        float qu      = c->texCoord.u - a->texCoord.u;
        float qv      = c->texCoord.v - a->texCoord.v;
        float det     = pu*qv - qu*pv;

        if( abs( det ) > 1e-6f ) {
          float detInv = det == 0.0f ? 0.0f : 1.0f / det;

          tangent   = detInv * Vec3( p.x*qv - q.x*pv, p.y*qv - q.y*pv, p.z*qv - q.z*pv );
          binormal  = detInv * Vec3( q.x*pu - p.x*qu, q.y*pu - p.y*qu, q.z*pu - p.z*qu );

          // Ortonormise.
          tangent  -= ( tangent * normal ) / normal.sqN() * normal;
          binormal -= ( binormal * normal ) / normal.sqN() * normal;
          binormal -= ( binormal * tangent ) / tangent.sqN() * tangent;

          if( tangent.sqN() == 0.0f || binormal.sqN() == 0.0f ) {
            tangent  = Vec3::ZERO;
            binormal = Vec3::ZERO;
          }
          else {
            tangent  = ~tangent;
            binormal = ~binormal;
          }
        }
      }

      v[j]->normal   = normal;
      v[j]->tangent  = tangent;
      v[j]->binormal = binormal;
    }
  }

  for( Vertex& vertex : vertices ) {
    if( nFrames != 0 ) {
      vertex.pos = Point( ( vertex.pos.x + 0.5f ) / float( nFramePositions ), 0.0f, 0.0f );
    }
    vertex.write( os );
  }
  for( ushort index : indices ) {
    os->writeUShort( index );
  }

  if( nFrames != 0 ) {
    hard_assert( positions.length() == nFrames * nFramePositions );
    hard_assert( normals.length() == nFrames * nFramePositions );

    for( const Point& position : positions ) {
      os->writePoint( position );
    }
    for( const Vec3& normal : normals ) {
      os->writeVec3( normal );
    }
  }

  for( const Mesh& mesh : meshes ) {
    os->writeInt( mesh.flags );
    os->writeInt( textures.index( mesh.texture ) );
    os->writeFloat( mesh.shininess );

    os->writeInt( mesh.nIndices );
    os->writeInt( mesh.firstIndex );
  }

  for( const Light& light : lights ) {
    hard_assert( nodes.index( light.node ) != -1 );

    os->writeInt( nodes.index( light.node ) );
    os->writeInt( light.type );

    os->writePoint( light.pos );
    os->writeVec3( light.dir );
    os->writeVec3( light.colour );

    os->writeFloat( light.attenuation[0] );
    os->writeFloat( light.attenuation[1] );
    os->writeFloat( light.attenuation[2] );

    os->writeFloat( light.coneCoeff[0] );
    os->writeFloat( light.coneCoeff[1] );
  }

  for( const Node* node : nodes ) {
    os->writeMat4( node->transf );
    os->writeInt( node->mesh );

    os->writeInt( nodes.index( node->parent ) );
    os->writeInt( node->firstChild );
    os->writeInt( node->nChildren );

    os->writeString( node->name );
  }

  for( const Animation& anim : animations ) {
    os->writeInt( anim.channels.length() );

    for( const Animation::Channel& channel : anim.channels ) {
      os->writeInt( channel.positionKeys.length() );
      os->writeInt( channel.rotationKeys.length() );
      os->writeInt( channel.scalingKeys.length() );

      for( const Animation::PositionKey& posKey : channel.positionKeys ) {
        os->writePoint( posKey.position );
        os->writeFloat( posKey.time );
      }
      for( const Animation::RotationKey& rotKey : channel.rotationKeys ) {
        os->writeQuat( rotKey.rotation );
        os->writeFloat( rotKey.time);
      }
      for( const Animation::ScalingKey& scaleKey : channel.scalingKeys ) {
        os->writeVec3( scaleKey.scaling );
        os->writeFloat( scaleKey.time );
      }
    }
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
  textures.exclude( String::EMPTY );

  for( int i = 0; i < textures.length(); ++i ) {
    context.buildTexture( textures[i], String( destDir, "/" ) + textures[i].fileName() );
  }
}

void Compiler::init()
{}

void Compiler::destroy()
{
  positions.clear();
  positions.trim();

  normals.clear();
  normals.trim();

  vertices.clear();
  vertices.trim();

  meshes.clear();
  meshes.trim();

  lights.clear();
  lights.trim();

  nodes.clear();
  nodes.trim();

  animations.clear();
  animations.trim();

  mesh.texture = "";
  mesh.indices.clear();
  mesh.indices.trim();

  root.children.free();
  Node::pool.free();

  polyIndices.clear();
  polyIndices.trim();

  hard_assert( channel.positionKeys.capacity() == 0 );
  hard_assert( channel.rotationKeys.capacity() == 0 );
  hard_assert( channel.scalingKeys.capacity() == 0 );

  hard_assert( animation.channels.capacity() == 0 );
}

Compiler compiler;

}
}
