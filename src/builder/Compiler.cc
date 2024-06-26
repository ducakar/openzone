/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <builder/Compiler.hh>

#include <builder/Context.hh>

using oz::client::Animation;

namespace oz::builder
{

namespace
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

  bool operator==(const Vertex& v) const
  {
    return pos == v.pos && texCoord == v.texCoord && normal == v.normal && tangent == v.tangent &&
           binormal == v.binormal && boneName[0] == v.boneName[0] && boneName[1] == v.boneName[1] &&
           weight[0] == v.weight[0] && weight[1] == v.weight[1];
  }

  void write(Stream* os) const
  {
    os->write<Point>(pos);

    os->writeInt16(int16(Math::lround(texCoord.u * 1024.0f)));
    os->writeInt16(int16(Math::lround(texCoord.v * 1024.0f)));

    os->writeByte(byte(normal.x * 127.0f));
    os->writeByte(byte(normal.y * 127.0f));
    os->writeByte(byte(normal.z * 127.0f));
    os->writeByte(0);

    os->writeByte(byte(tangent.x * 127.0f));
    os->writeByte(byte(tangent.y * 127.0f));
    os->writeByte(byte(tangent.z * 127.0f));
    os->writeByte(0);

    os->writeByte(byte(binormal.x * 127.0f));
    os->writeByte(byte(binormal.y * 127.0f));
    os->writeByte(byte(binormal.z * 127.0f));
    os->writeByte(0);

    os->writeUByte(ubyte(bone[0]));
    os->writeUByte(ubyte(bone[1]));
    os->writeUByte(ubyte(weight[0] * 255.0f));
    os->writeUByte(ubyte(weight[1] * 255.0f));
  }
};

struct Triangle
{
  float depth;
  int   index;

  bool operator<(const Triangle& t) const
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
  List<uint16> indices;
};

struct Light
{
  Node*               node;
  client::Light::Type type;

  Point               pos;
  Vec3                dir;
  Vec3                colour;
  float               attenuation[3];
  float               coneCoeff[2];
};

struct Node
{
  static Pool<Node> pool;

  Mat4        transf;
  int         firstChild;
  int         nChildren;
  int         mesh = -1;
  String      name;

  bool        includeInBounds = true;

  Node*       parent;
  List<Node*> children;

  Node*       prev[1];
  Node*       next[1];

  explicit Node(const char* name_ = "", Node* parent_ = nullptr)
    : transf(Mat4::ID), name(name_), parent(parent_)
  {}

  ~Node()
  {
    children.free();
  }

  OZ_NO_COPY(Node)
  OZ_GENERIC_MOVE(Node)

  OZ_STATIC_POOL_ALLOC(pool)
};

Pool<Node>         Node::pool;

List<Point>        positions;
List<Vec3>         normals;
List<Vertex>       vertices;
List<Mesh>         meshes;
List<Light>        lights;
List<Node*>        nodes;
List<Animation>    animations;

Bounds             bounds;

Vertex             currentVert;
Mesh               currentMesh;
Light              currentLlight;
Node               root;
Node*              currentNode;
Animation::Channel currentChannel;
Animation          animation;

Environment        environment;
int                caps;
String             shaderName;
int                nFrames;
int                nFramePositions;
Compiler::PolyMode mode;
int                vertNum;
List<uint16>       polyIndices;

void calculateBounds(const Node* node, const Mat4& parentTransf)
{
  Mat4 transf = parentTransf ^ node->transf;

  if (node->includeInBounds && node->mesh != -1) {
    const Mesh& mesh = meshes[node->mesh];

    for (int index : mesh.indices) {
      Point pos = vertices[index].pos;

      if (nFrames != 0) {
        pos = positions[int(pos.x)];
      }

      pos = transf * pos;

      bounds.mins = min(bounds.mins, pos);
      bounds.maxs = max(bounds.maxs, pos);
    }
  }

  for (const Node* child : node->children) {
    calculateBounds(child, transf);
  }
}

void storeNode(Node* node, int depth)
{
  if (depth != 0) {
    for (Node* child : node->children) {
      storeNode(child, depth - 1);
    }
  }
  else {
    node->firstChild = nodes.size();
    node->nChildren  = node->children.size();

    nodes.addAll(node->children.begin(), node->children.size());
  }
}

}

void Compiler::enable(Capability cap)
{
  caps |= cap;
}

void Compiler::disable(Capability cap)
{
  caps &= ~cap;
}

void Compiler::beginModel()
{
  OZ_ASSERT(environment == NONE);

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

  bounds.mins = Point(+Math::INF, +Math::INF, +Math::INF);
  bounds.maxs = Point(-Math::INF, -Math::INF, -Math::INF);

  currentMesh.flags   = Model::SOLID_BIT;
  currentMesh.texture = "";

  currentLlight.pos            = Point::ORIGIN;
  currentLlight.dir            = Vec3::ZERO;
  currentLlight.colour         = Vec3::ZERO;
  currentLlight.coneCoeff[0]   = 0.0f;
  currentLlight.coneCoeff[1]   = 0.0f;
  currentLlight.attenuation[0] = 0.0f;
  currentLlight.attenuation[1] = 0.0f;
  currentLlight.attenuation[2] = 0.0f;
  currentLlight.type           = client::Light::POINT;

  root        = Node("");
  currentNode = &root;

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
  OZ_ASSERT(environment == MODEL);
  environment = NONE;

  calculateBounds(&root, Mat4::ID);
}

void Compiler::shader(const char* shaderName_)
{
  OZ_ASSERT(environment == MODEL);

  shaderName = shaderName_;
}

void Compiler::anim(int nFrames_, int nFramePositions_)
{
  OZ_ASSERT(environment == MODEL);

  if (nFrames <= 1) {
    nFrames         = 0;
    nFramePositions = 0;
  }

  nFrames         = nFrames_;
  nFramePositions = nFramePositions_;

  positions.resize(nFrames * nFramePositions);
  normals.resize(nFrames * nFramePositions);
}

void Compiler::animPositions(const float* positions_)
{
  OZ_ASSERT(environment == MODEL);
  OZ_ASSERT(nFrames != 0);

  for (Point& position : positions) {
    position.x = *positions_++;
    position.y = *positions_++;
    position.z = *positions_++;
  }
}

void Compiler::animNormals(const float* normals_)
{
  OZ_ASSERT(environment == MODEL);
  OZ_ASSERT(nFrames != 0);

  for (Vec3& normal : normals) {
    normal.x = *normals_++;
    normal.y = *normals_++;
    normal.z = *normals_++;
  }
}

void Compiler::beginNode(const char* name)
{
  OZ_ASSERT(environment == MODEL);

  Node* newNode = new Node(name, currentNode);

  currentNode->children.add(newNode);
  currentNode = newNode;
}

void Compiler::endNode()
{
  OZ_ASSERT(environment == MODEL && currentNode != &root);

  currentNode = currentNode->parent;
}

void Compiler::transform(const Mat4& t)
{
  OZ_ASSERT(environment == MODEL && currentNode != &root);

  currentNode->transf = t;
}

void Compiler::includeInBounds(bool value)
{
  OZ_ASSERT(environment == MODEL && currentNode != &root);

  currentNode->includeInBounds = value;
}

void Compiler::bindMesh(int id)
{
  OZ_ASSERT(environment == MODEL && currentNode != &root);

  currentNode->mesh = id;
}

void Compiler::bindLight(int id)
{
  OZ_ASSERT(environment == MODEL && currentNode != &root);

  lights[id].node = currentNode;
}

void Compiler::beginMesh()
{
  OZ_ASSERT(environment == MODEL);
  environment = MESH;

  currentMesh.flags     = Model::SOLID_BIT;
  currentMesh.texture   = "";
  currentMesh.shininess = 50.0f;
}

int Compiler::endMesh()
{
  OZ_ASSERT(environment == MESH);
  environment = MODEL;

  meshes.add(currentMesh);
  currentMesh.indices.clear();

  return meshes.size() - 1;
}

void Compiler::texture(const char* texture)
{
  OZ_ASSERT(environment == MESH);

  currentMesh.texture = texture;
}

void Compiler::shininess(float exponent)
{
  currentMesh.shininess = exponent;
}

void Compiler::blend(bool doBlend)
{
  OZ_ASSERT(environment == MESH);

  currentMesh.flags = doBlend ? Model::ALPHA_BIT : Model::SOLID_BIT;
}

void Compiler::begin(Compiler::PolyMode mode_)
{
  OZ_ASSERT(environment == MESH);
  environment = POLY;

  currentVert.pos         = Point::ORIGIN;
  currentVert.texCoord    = TexCoord(0.0f, 0.0f);
  currentVert.normal      = Vec3::ZERO;
  currentVert.tangent     = Vec3::ZERO;
  currentVert.binormal    = Vec3::ZERO;
  currentVert.boneName[0] = "";
  currentVert.boneName[1] = "";
  currentVert.bone[0]     = 0;
  currentVert.bone[1]     = 0;
  currentVert.weight[0]   = 0.0f;
  currentVert.weight[1]   = 0.0f;

  mode             = mode_;
  vertNum          = 0;
  polyIndices.clear();
}

void Compiler::end()
{
  OZ_ASSERT(environment == POLY);
  environment = MESH;

  if (caps & CLOCKWISE) {
    polyIndices.reverse();
  }

  switch (mode) {
    case TRIANGLES: {
      OZ_ASSERT(vertNum >= 3 && vertNum % 3 == 0);

      currentMesh.indices.takeAll(polyIndices.begin(), polyIndices.size());
      break;
    }
    case POLYGON: {
      OZ_ASSERT(vertNum >= 3);

      int last[2] = {0, 1};
      int top     = vertNum - 1;
      int bottom  = 2;

      for (int i = 0; bottom <= top; ++i) {
        if (i & 1) {
          currentMesh.indices.add(polyIndices[last[0]]);
          currentMesh.indices.add(polyIndices[last[1]]);
          currentMesh.indices.add(polyIndices[bottom]);

          last[1] = bottom;
          ++bottom;
        }
        else {
          currentMesh.indices.add(polyIndices[last[0]]);
          currentMesh.indices.add(polyIndices[last[1]]);
          currentMesh.indices.add(polyIndices[top]);

          last[0] = top;
          --top;
        }
      }
      break;
    }
  }
}

void Compiler::boneWeight(int which, const char* name, float weight)
{
  OZ_ASSERT(environment == POLY);
  OZ_ASSERT(which == 0 || which == 1);

  currentVert.boneName[which] = name;
  currentVert.weight[which]   = weight;
}

void Compiler::texCoord(float u, float v)
{
  OZ_ASSERT(environment == POLY);

  currentVert.texCoord[0] = u;
  currentVert.texCoord[1] = v;
}

void Compiler::texCoord(const float* v)
{
  texCoord(v[0], v[1]);
}

void Compiler::normal(float x, float y, float z)
{
  OZ_ASSERT(environment == POLY);

  currentVert.normal.x = x;
  currentVert.normal.y = y;
  currentVert.normal.z = z;
}

void Compiler::normal(const float* v)
{
  normal(v[0], v[1], v[2]);
}

void Compiler::vertex(float x, float y, float z)
{
  OZ_ASSERT(environment == POLY);
  OZ_ASSERT(nFrames == 0 || (y == 0.0f && z == 0.0f));

  currentVert.pos.x = x;
  currentVert.pos.y = y;
  currentVert.pos.z = z;

  int index = 0;

  if (caps & UNIQUE) {
    index = int(&vertices.include(currentVert) - vertices.begin());

    polyIndices.add(uint16(index));
  }
  else {
    index = vertices.size();

    vertices.add(currentVert);
    polyIndices.add(uint16(index));
  }

  ++vertNum;
}

void Compiler::vertex(const float* v)
{
  vertex(v[0], v[1], v[2]);
}

void Compiler::animVertex(int i)
{
  OZ_ASSERT(environment == POLY);
  OZ_ASSERT(nFrames > 1 && uint(i) < uint(positions.size()));

  OZ_ASSERT(normals[i].fastN() > 0.9f);
  normal(normals[i]);
  vertex(float(i), 0.0f, 0.0f);
}

void Compiler::beginLight(client::Light::Type type)
{
  OZ_ASSERT(environment == MODEL);
  environment = LIGHT;

  currentLlight.type = type;
}

int Compiler::endLight()
{
  OZ_ASSERT(environment == LIGHT);
  environment = MODEL;

  lights.add(currentLlight);
  return lights.size() - 1;
}

void Compiler::position(float x, float y, float z)
{
  OZ_ASSERT(environment == LIGHT);

  currentLlight.pos.x = x;
  currentLlight.pos.y = y;
  currentLlight.pos.z = z;
}

void Compiler::direction(float x, float y, float z)
{
  OZ_ASSERT(environment == LIGHT);

  currentLlight.dir.x = x;
  currentLlight.dir.y = y;
  currentLlight.dir.z = z;
}

void Compiler::colour(float r, float g, float b)
{
  OZ_ASSERT(environment == LIGHT);

  currentLlight.colour.x = r;
  currentLlight.colour.y = g;
  currentLlight.colour.z = b;
}

void Compiler::attenuation(float constant, float linear, float quadratic)
{
  OZ_ASSERT(environment == LIGHT);

  currentLlight.attenuation[0] = constant;
  currentLlight.attenuation[1] = linear;
  currentLlight.attenuation[2] = quadratic;
}

void Compiler::coneAngles(float inner, float outer)
{
  OZ_ASSERT(environment == LIGHT);

  currentLlight.coneCoeff[0] = Math::tan(inner / 2.0f);
  currentLlight.coneCoeff[1] = Math::tan(outer / 2.0f);
}

void Compiler::beginAnimation()
{
  OZ_ASSERT(environment == MODEL);
  environment = ANIMATION;

  animation.channels.clear();
}

void Compiler::endAnimation()
{
  OZ_ASSERT(environment == ANIMATION);
  environment = MODEL;

  animations.add(static_cast<Animation&&>(animation));
}

void Compiler::beginChannel()
{
  OZ_ASSERT(environment == ANIMATION);
  environment = CHANNEL;

  currentChannel.positionKeys.clear();
  currentChannel.rotationKeys.clear();
  currentChannel.scalingKeys.clear();
}

void Compiler::endChannel()
{
  OZ_ASSERT(environment == CHANNEL);
  environment = ANIMATION;

  animation.channels.add(static_cast<Animation::Channel&&>(currentChannel));
}

void Compiler::positionKey(const Point& pos, float time)
{
  OZ_ASSERT(environment == CHANNEL);

  currentChannel.positionKeys.add(Animation::PositionKey{pos, time});
}

void Compiler::rotationKey(const Quat& rot, float time)
{
  OZ_ASSERT(environment == CHANNEL);

  currentChannel.rotationKeys.add(Animation::RotationKey{rot, time});
}

void Compiler::scalingKey(const Vec3& scale, float time)
{
  OZ_ASSERT(environment == CHANNEL);

  currentChannel.scalingKeys.add(Animation::ScalingKey{scale, time});
}

void Compiler::writeModel(Stream* os, bool globalTextures)
{
  OZ_ASSERT(environment == NONE);
  OZ_ASSERT(meshes.size() > 0 && vertices.size() > 0);
  OZ_ASSERT(positions.size() == normals.size());

  Log::print("Writing mesh ...");

  List<String> textures;
  List<uint16> indices;

  int nIndices = 0;

  for (Mesh& mesh : meshes) {
    textures.include(mesh.texture);

    mesh.firstIndex = nIndices;
    mesh.nIndices   = mesh.indices.size();

    indices.addAll(mesh.indices.begin(), mesh.indices.size());

    nIndices += mesh.nIndices;
  }

  int nNodes = -1;

  for (int i = 0; nNodes != nodes.size(); ++i) {
    nNodes = nodes.size();

    storeNode(&root, i);
  }

  // Size in bytes, will be written at the end.
  os->writeInt(0);

  os->write<Vec3>(bounds.dim());

  os->writeString(shaderName);
  os->writeInt(globalTextures ? ~textures.size() : textures.size());
  os->writeInt(vertices.size());
  os->writeInt(nIndices);
  os->writeInt(nFrames);
  os->writeInt(nFramePositions);

  os->writeInt(meshes.size());
  os->writeInt(lights.size());
  os->writeInt(Node::pool.size());
  os->writeInt(animations.size());

  for (const String& texture : textures) {
    os->writeString(texture);
  }

  // Generate tangents and binormals.
  for (int i = 0; i < indices.size(); i += 3) {
    Vertex* v[3] = {
      &vertices[indices[i + 0]],
      &vertices[indices[i + 1]],
      &vertices[indices[i + 2]]
    };

    // [t_x b_x]   [p_x q_x]            -1
    // [t_y b_y] = [p_y q_y] [p_u q_u]
    // [t_z b_z]   [p_z q_z] [p_v q_v]
    //
    // (t, b) - tangent and binormal (non-ortonormised)
    // (p, q) - delta points
    for (int j = 0; j < 3; ++j) {
      Vec3 normal   = v[j]->normal;
      Vec3 tangent  = Vec3::ZERO;
      Vec3 binormal = Vec3::ZERO;

      if (normal.sqN() != 0.0f) {
        Vertex* a     = v[j];
        Vertex* b     = v[(j + 1) % 3];
        Vertex* c     = v[(j + 2) % 3];

        Vec3  p       = b->pos - a->pos;
        Vec3  q       = c->pos - a->pos;
        float pu      = b->texCoord.u - a->texCoord.u;
        float pv      = b->texCoord.v - a->texCoord.v;
        float qu      = c->texCoord.u - a->texCoord.u;
        float qv      = c->texCoord.v - a->texCoord.v;
        float det     = pu*qv - qu*pv;

        if (abs(det) > 1e-6f) {
          float detInv = det == 0.0f ? 0.0f : 1.0f / det;

          tangent   = detInv * Vec3(p.x*qv - q.x*pv, p.y*qv - q.y*pv, p.z*qv - q.z*pv);
          binormal  = detInv * Vec3(q.x*pu - p.x*qu, q.y*pu - p.y*qu, q.z*pu - p.z*qu);

          // Ortonormise.
          tangent  -= (tangent * normal) / normal.sqN() * normal;
          binormal -= (binormal * normal) / normal.sqN() * normal;
          binormal -= (binormal * tangent) / tangent.sqN() * tangent;

          if (tangent.sqN() == 0.0f || binormal.sqN() == 0.0f) {
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

  for (Vertex& vertex : vertices) {
    if (nFrames != 0) {
      vertex.pos = Point((vertex.pos.x + 0.5f) / float(nFramePositions), 0.0f, 0.0f);
    }
    vertex.write(os);
  }
  for (uint16 index : indices) {
    os->writeUInt16(index);
  }

  if (nFrames != 0) {
    OZ_ASSERT(positions.size() == nFrames * nFramePositions);
    OZ_ASSERT(normals.size() == nFrames * nFramePositions);

    for (const Point& position : positions) {
      os->write<Point>(position);
    }
    for (const Vec3& normal : normals) {
      os->write<Vec3>(normal);
    }
  }

  for (const Mesh& mesh : meshes) {
    os->writeInt(mesh.flags);
    os->writeInt(textures.index(mesh.texture));
    os->writeFloat(mesh.shininess);

    os->writeInt(mesh.nIndices);
    os->writeInt(mesh.firstIndex);
  }

  for (const Light& light : lights) {
    OZ_ASSERT(nodes.index(light.node) != -1);

    os->writeInt(nodes.index(light.node));
    os->writeInt(light.type);

    os->write<Point>(light.pos);
    os->write<Vec3>(light.dir);
    os->write<Vec3>(light.colour);

    os->writeFloat(light.attenuation[0]);
    os->writeFloat(light.attenuation[1]);
    os->writeFloat(light.attenuation[2]);

    os->writeFloat(light.coneCoeff[0]);
    os->writeFloat(light.coneCoeff[1]);
  }

  for (const Node* node : nodes) {
    os->write<Mat4>(node->transf);
    os->writeInt(node->mesh);

    os->writeInt(nodes.index(node->parent));
    os->writeInt(node->firstChild);
    os->writeInt(node->nChildren);

    os->writeString(node->name);
  }

  for (const Animation& anim : animations) {
    os->writeInt(anim.channels.size());

    for (const Animation::Channel& channel : anim.channels) {
      os->writeInt(channel.positionKeys.size());
      os->writeInt(channel.rotationKeys.size());
      os->writeInt(channel.scalingKeys.size());

      for (const Animation::PositionKey& posKey : channel.positionKeys) {
        os->write<Point>(posKey.position);
        os->writeFloat(posKey.time);
      }
      for (const Animation::RotationKey& rotKey : channel.rotationKeys) {
        os->write<Quat>(rotKey.rotation);
        os->writeFloat(rotKey.time);
      }
      for (const Animation::ScalingKey& scaleKey : channel.scalingKeys) {
        os->write<Vec3>(scaleKey.scaling);
        os->writeFloat(scaleKey.time);
      }
    }
  }

  // Write size of the model in bytes at the beginning.
  int size = os->tell();

  os->rewind();
  os->writeInt(size);

  os->seek(size);

  Log::printEnd(" OK");
}

void Compiler::buildModelTextures(const File& destDir)
{
  OZ_ASSERT(environment == NONE);

  List<String> textures;

  for (const Mesh& mesh : meshes) {
    textures.include(mesh.texture);
  }
  textures.exclude(String::EMPTY);

  for (const String& texture : textures) {
    context.buildTexture(texture, destDir / File(texture).name());
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

  currentMesh.texture = "";
  currentMesh.indices.clear();
  currentMesh.indices.trim();

  root.children.free();
  Node::pool.free();

  polyIndices.clear();
  polyIndices.trim();

  OZ_ASSERT(currentChannel.positionKeys.capacity() == 0);
  OZ_ASSERT(currentChannel.rotationKeys.capacity() == 0);
  OZ_ASSERT(currentChannel.scalingKeys.capacity() == 0);

  OZ_ASSERT(animation.channels.capacity() == 0);
}

Compiler compiler;

}
