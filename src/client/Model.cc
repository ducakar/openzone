/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include <client/Model.hh>

#include <client/Terra.hh>
#include <client/Context.hh>
#include <client/Camera.hh>

namespace oz::client
{

void Vertex::setFormat()
{
  glEnableVertexAttribArray(Shader::POSITION);
  glVertexAttribPointer(Shader::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        static_cast<char*>(nullptr) + offsetof(Vertex, pos));

  glEnableVertexAttribArray(Shader::TEXCOORD);
  glVertexAttribPointer(Shader::TEXCOORD, 2, GL_SHORT, GL_FALSE, sizeof(Vertex),
                        static_cast<char*>(nullptr) + offsetof(Vertex, texCoord));

  glEnableVertexAttribArray(Shader::NORMAL);
  glVertexAttribPointer(Shader::NORMAL, 3, GL_BYTE, GL_TRUE, sizeof(Vertex),
                        static_cast<char*>(nullptr) + offsetof(Vertex, normal));

  glEnableVertexAttribArray(Shader::TANGENT);
  glVertexAttribPointer(Shader::TANGENT, 3, GL_BYTE, GL_TRUE, sizeof(Vertex),
                        static_cast<char*>(nullptr) + offsetof(Vertex, tangent));

  glEnableVertexAttribArray(Shader::BINORMAL);
  glVertexAttribPointer(Shader::BINORMAL, 3, GL_BYTE, GL_TRUE, sizeof(Vertex),
                        static_cast<char*>(nullptr) + offsetof(Vertex, binormal));
}

struct Model::LightEntry
{
  const Light* light;
  Mat4         transf;
  float        weight;
};

struct Model::PreloadData
{
  struct TexFiles
  {
    File albedo;
    File masks;
    File normals;
  };

  File           modelFile;
  List<TexFiles> textures;
};

Set<Model::Ref>         Model::loadedModels;
List<Model::Instance>   Model::instances[2];
List<Model::LightEntry> Model::sceneLights;
Vertex*                 Model::vertexAnimBuffer       = nullptr;
int                     Model::vertexAnimBufferLength = 0;
Model::Collation        Model::collation              = DEPTH_MAJOR;

void Model::addSceneLights()
{
  for (const Light& light : lights) {
    const Node* node = &nodes[light.node];
    Mat4 transf = node->transf;

    while (node->parent != -1) {
      node   = &nodes[node->parent];
      transf = node->transf ^ transf;
    }

    sceneLights.add(LightEntry{&light, transf, 0.0f});
  }
}

void Model::animate(const Instance* instance)
{
  if (shader.hasVTF) {
    glActiveTexture(Shader::VERTEX_ANIM);
    glBindTexture(GL_TEXTURE_2D, animationTexId);

    glUniform3f(uniform.meshAnimation,
                float(instance->firstFrame) / float(nFrames),
                float(instance->secondFrame) / float(nFrames),
                instance->interpolation);
  }
  else {
    const Point* currFramePositions = &positions[instance->firstFrame * nFramePositions];
    const Vec3*  currFrameNormals   = &normals[instance->firstFrame * nFramePositions];

    if (instance->interpolation == 0.0f) {
      for (int i = 0; i < nVertices; ++i) {
        int j = Math::lround(vertices[i].pos[0] * float(nFramePositions - 1));

        Point pos    = currFramePositions[j];
        Vec3  normal = currFrameNormals[j];

        vertexAnimBuffer[i].pos[0] = pos.x;
        vertexAnimBuffer[i].pos[1] = pos.y;
        vertexAnimBuffer[i].pos[2] = pos.z;

        vertexAnimBuffer[i].texCoord[0] = vertices[i].texCoord[0];
        vertexAnimBuffer[i].texCoord[1] = vertices[i].texCoord[1];

        vertexAnimBuffer[i].normal[0] = byte(normal.x * 127.0f);
        vertexAnimBuffer[i].normal[1] = byte(normal.y * 127.0f);
        vertexAnimBuffer[i].normal[2] = byte(normal.z * 127.0f);
      }
    }
    else {
      const Point* nextFramePositions = &positions[instance->secondFrame * nFramePositions];
      const Vec3*  nextFrameNormals   = &normals[instance->secondFrame * nFramePositions];

      for (int i = 0; i < nVertices; ++i) {
        int j = Math::lround(vertices[i].pos[0] * float(nFramePositions - 1));

        Point pos    = Math::mix(currFramePositions[j], nextFramePositions[j], instance->interpolation);
        Vec3  normal = Math::mix(currFrameNormals[j],   nextFrameNormals[j],   instance->interpolation);

        vertexAnimBuffer[i].pos[0] = pos.x;
        vertexAnimBuffer[i].pos[1] = pos.y;
        vertexAnimBuffer[i].pos[2] = pos.z;

        vertexAnimBuffer[i].texCoord[0] = vertices[i].texCoord[0];
        vertexAnimBuffer[i].texCoord[1] = vertices[i].texCoord[1];

        vertexAnimBuffer[i].normal[0] = byte(normal.x * 127.0f);
        vertexAnimBuffer[i].normal[1] = byte(normal.y * 127.0f);
        vertexAnimBuffer[i].normal[2] = byte(normal.z * 127.0f);
      }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(Vertex), vertexAnimBuffer, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

void Model::drawNode(const Node* node, int mask)
{
  tf.push();
  tf.model = tf.model ^ node->transf;

  if (node->mesh != -1) {
    const Mesh& mesh = meshes[node->mesh];

    if (mesh.flags & mask) {
      const Texture& texture = textures[mesh.texture];

      tf.apply();

      glActiveTexture(Shader::DIFFUSE);
      glBindTexture(GL_TEXTURE_2D, texture.albedo);
      glActiveTexture(Shader::MASKS);
      glBindTexture(GL_TEXTURE_2D, texture.masks);
      if (shader.doBumpMap) {
        glActiveTexture(Shader::NORMALS);
        glBindTexture(GL_TEXTURE_2D, texture.normals);
      }

      glUniform1f(uniform.shininess, mesh.shininess);

      glDrawElements(GL_TRIANGLES, mesh.nIndices, GL_UNSIGNED_SHORT,
                     static_cast<uint16*>(nullptr) + mesh.firstIndex);
    }
  }

  for (int i = 0; i < node->nChildren; ++i) {
    drawNode(&nodes[node->firstChild + i], mask);
  }

  tf.pop();
}

void Model::draw(const Instance* instance, int mask)
{
  tf.model  = instance->transf;
  tf.colour = instance->colour;
  tf.applyColour();

  drawNode(&nodes[instance->node], mask);
}

void Model::setCollation(Collation collation_)
{
  collation = collation_;
}

void Model::drawScheduled(QueueType queue, int mask)
{
  if (collation == MODEL_MAJOR) {
    for (const Ref& ref : loadedModels) {
      Model* model = ref.model;

      if (model->modelInstances[queue].isEmpty()) {
        continue;
      }

      glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);

      Vertex::setFormat();

      shader.program(model->shaderId);

      for (const Instance& instance : model->modelInstances[queue]) {
        // HACK This is not a nice way to draw non-transparent parts with alpha < 1.
        int instanceMask = mask;

        if (instance.colour.w.w != 1.0f) {
          if (mask & ALPHA_BIT) {
            instanceMask |= SOLID_BIT;
          }
          else {
            continue;
          }
        }

        if (!(model->flags & instanceMask)) {
          continue;
        }

        if (model->nFrames != 0) {
          model->animate(&instance);
        }

        model->draw(&instance, instanceMask);
      }
    }
  }
  else {
    Model* model = nullptr;

    for (const Instance& instance : instances[queue]) {
      if (instance.model != model) {
        model = instance.model;

        glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);

        Vertex::setFormat();

        shader.program(model->shaderId);
      }

      // HACK This is not a nice way to draw non-transparent parts for which alpha < 1 has been set.
      int instanceMask = mask;

      if (instance.colour.w.w != 1.0f) {
        if (mask & ALPHA_BIT) {
          instanceMask |= SOLID_BIT;
        }
        else {
          continue;
        }
      }

      if (!(model->flags & instanceMask)) {
        continue;
      }

      if (model->nFrames != 0) {
        model->animate(&instance);
      }

      model->draw(&instance, instanceMask);
    }
  }

  if (shader.hasVTF) {
    glActiveTexture(Shader::VERTEX_ANIM);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  glActiveTexture(Shader::NORMALS);
  glBindTexture(GL_TEXTURE_2D, shader.defaultNormals);

  glActiveTexture(Shader::MASKS);
  glBindTexture(GL_TEXTURE_2D, shader.defaultMasks);

  glActiveTexture(Shader::DIFFUSE);
  glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glFlush();
}

void Model::clearScheduled(QueueType queue)
{
  if (collation == MODEL_MAJOR) {
    for (const Ref& ref : loadedModels) {
      ref.model->modelInstances[queue].clear();
    }
  }
  else {
    instances[queue].clear();
  }

  sceneLights.clear();
}

void Model::deallocate()
{
  OZ_ASSERT(loadedModels.isEmpty());

  loadedModels.trim();

  delete[] vertexAnimBuffer;
  vertexAnimBuffer = nullptr;
  vertexAnimBufferLength = 0;

  instances[SCENE_QUEUE].trim();
  instances[OVERLAY_QUEUE].trim();

  sceneLights.trim();
}

Model::Model(const char* path_)
  : path(path_), vbo(0), ibo(0), animationTexId(0),
    nTextures(0), nVertices(0), nIndices(0), nFrames(0), nFramePositions(0),
    vertices(nullptr), positions(nullptr), normals(nullptr),
    preloadData(nullptr), dim(Vec3::ONE), size(dim.fastN())
{}

Model::~Model()
{
  unload();
}

int Model::findNode(const char* name) const
{
  for (int i = 0; i < nodes.size(); ++i) {
    if (nodes[i].name == name) {
      return i;
    }
  }
  return -1;
}

void Model::schedule(int mesh, QueueType queue)
{
  List<Instance>& list = collation == MODEL_MAJOR ? modelInstances[queue] : instances[queue];

  if (shader.nLights != 0 && lights.isEmpty() != 0) {
    addSceneLights();
  }

  list.add(Instance{this, tf.model, tf.colour, mesh, 0, 0, 0.0f});
}

void Model::scheduleFrame(int mesh, int frame, QueueType queue)
{
  List<Instance>& list = collation == MODEL_MAJOR ? modelInstances[queue] : instances[queue];

  if (shader.nLights != 0 && lights.isEmpty() != 0) {
    addSceneLights();
  }

  list.add(Instance{this, tf.model, tf.colour, mesh, frame, 0, 0.0f});
}

void Model::scheduleAnimated(int mesh, int firstFrame, int secondFrame, float interpolation,
                             QueueType queue)
{
  List<Instance>& list = collation == MODEL_MAJOR ? modelInstances[queue] : instances[queue];

  if (shader.nLights != 0 && lights.isEmpty() != 0) {
    addSceneLights();
  }

  list.add(Instance{this, tf.model, tf.colour, mesh, firstFrame, secondFrame, interpolation});
}

const File* Model::preload()
{
  OZ_ASSERT(preloadData == nullptr);

  Stream is(0, Endian::LITTLE);

  preloadData = new PreloadData();
  preloadData->modelFile = path;

  if (!preloadData->modelFile.read(&is)) {
    OZ_ERROR("Failed to read '%s'", path.c());
  }

  is.readInt();

  dim             = is.read<Vec3>();
  size            = dim.fastN();
  flags           = 0;
  shaderId        = liber.shaderIndex(is.readString());
  nTextures       = is.readInt();
  nVertices       = is.readInt();
  nIndices        = is.readInt();
  nFrames         = is.readInt();
  nFramePositions = is.readInt();

  int nMeshes     = is.readInt();
  int nLights     = is.readInt();
  int nNodes      = is.readInt();
  int nAnimations = is.readInt();

  if (nTextures < 0) {
    textures.resize(~nTextures, true);

    for (int i = 0; i < ~nTextures; ++i) {
      is.readString();
    }
  }
  else {
    OZ_ASSERT(nTextures > 0);

    textures.resize(nTextures, true);

    for (int i = 0; i < nTextures; ++i) {
      String name = is.readString();

      preloadData->textures.add(PreloadData::TexFiles());

      if (!name.isEmpty()) {
        PreloadData::TexFiles& texFiles = preloadData->textures.last();

        texFiles.albedo  = name + ".dds";
        texFiles.masks   = name + "_m.dds";
        texFiles.normals = name + "_n.dds";
      }
    }
  }

  int vboSize = nVertices * sizeof(Vertex);
  int iboSize = nIndices  * sizeof(uint16);

  const void* vertexBuffer = is.readSkip(vboSize);
  is.readSkip(iboSize);

  if (nFrames != 0) {
    if (shader.hasVTF) {
      int vertexBufferSize = nFramePositions * nFrames * sizeof(float[3]);
      int normalBufferSize = nFramePositions * nFrames * sizeof(float[3]);

      is.readSkip(vertexBufferSize + normalBufferSize);
    }
    else {
      vertices  = new Vertex[nVertices];
      positions = new Point[nFramePositions * nFrames];
      normals   = new Vec3[nFramePositions * nFrames];

      for (int i = 0; i < nFramePositions * nFrames; ++i) {
        positions[i] = is.read<Point>();
      }
      for (int i = 0; i < nFramePositions * nFrames; ++i) {
        normals[i] = is.read<Vec3>();
      }

      memcpy(vertices, vertexBuffer, nVertices * sizeof(Vertex));
    }
  }

  meshes.resize(nMeshes, true);

  for (int i = 0; i < nMeshes; ++i) {
    meshes[i].flags      = is.readInt();
    meshes[i].texture    = is.readInt();
    meshes[i].shininess  = is.readFloat();

    meshes[i].nIndices   = is.readInt();
    meshes[i].firstIndex = is.readInt();

    flags |= meshes[i].flags & (SOLID_BIT | ALPHA_BIT);
  }

  lights.resize(nLights, true);

  for (int i = 0; i < nLights; ++i) {
    lights[i].node           = is.readInt();
    lights[i].type           = Light::Type(is.readInt());

    lights[i].pos            = is.read<Point>();
    lights[i].dir            = is.read<Vec3>();
    lights[i].colour         = is.read<Vec3>();

    lights[i].attenuation[0] = is.readFloat();
    lights[i].attenuation[1] = is.readFloat();
    lights[i].attenuation[2] = is.readFloat();

    lights[i].coneCoeff[0]   = is.readFloat();
    lights[i].coneCoeff[1]   = is.readFloat();
  }

  nodes.resize(nNodes, true);

  for (int i = 0; i < nNodes; ++i) {
    nodes[i].transf     = is.read<Mat4>();
    nodes[i].mesh       = is.readInt();

    nodes[i].parent     = is.readInt();
    nodes[i].firstChild = is.readInt();
    nodes[i].nChildren  = is.readInt();

    nodes[i].name       = is.readString();
  }

  animations.resize(nAnimations, true);

  for (int i = 0; i < nAnimations; ++i) {
    int nChannels = is.readInt();

    animations[i].channels.resize(nChannels, true);

    for (int j = 0; j < nChannels; ++j) {
      Animation::Channel& channel = animations[i].channels[j];

      int nPositionKeys = is.readInt();
      int nRotationKeys = is.readInt();
      int nScalingKeys  = is.readInt();

      channel.positionKeys.resize(nPositionKeys, true);
      channel.rotationKeys.resize(nRotationKeys, true);
      channel.scalingKeys.resize(nScalingKeys, true);

      for (int k = 0; k < nPositionKeys; ++k) {
        channel.positionKeys[k].position = is.read<Point>();
        channel.positionKeys[k].time     = is.readFloat();
      }
      for (int k = 0; k < nRotationKeys; ++k) {
        channel.rotationKeys[k].rotation = is.read<Quat>();
        channel.rotationKeys[k].time     = is.readFloat();
      }
      for (int k = 0; k < nScalingKeys; ++k) {
        channel.scalingKeys[k].scaling = is.read<Vec3>();
        channel.scalingKeys[k].time    = is.readFloat();
      }
    }
  }

  return &preloadData->modelFile;
}

void Model::load()
{
  OZ_NACL_IS_MAIN(true);
  OZ_ASSERT(preloadData != nullptr);

  Stream is(0, Endian::LITTLE);
  if (!preloadData->modelFile.read(&is)) {
    OZ_ERROR("Failed to read '%s'", preloadData->modelFile.c());
  }

  is.readInt();
  is.read<Vec3>();
  is.readString();
  is.readInt();
  is.readInt();
  is.readInt();
  is.readInt();
  is.readInt();

  is.readInt();
  is.readInt();
  is.readInt();
  is.readInt();

  if (nTextures < 0) {
    nTextures = ~nTextures;

    for (int i = 0; i < nTextures; ++i) {
      String name = is.readString();

      int id = name.beginsWith("@sea:") ? terra.liquidTexId : liber.textureIndex(name);
      textures[i] = context.requestTexture(id);
    }
  }
  else {
    for (int i = 0; i < nTextures; ++i) {
      is.readString();

      textures[i] = context.loadTexture(preloadData->textures[i].albedo,
                                        preloadData->textures[i].masks,
                                        preloadData->textures[i].normals);
    }
  }

  uint usage   = nFrames != 0 && shader.hasVTF ? GL_STREAM_DRAW : GL_STATIC_DRAW;
  int  vboSize = nVertices * sizeof(Vertex);
  int  iboSize = nIndices  * sizeof(uint16);

  const void* vertexBuffer = is.readSkip(vboSize);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vboSize, vertexBuffer, usage);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboSize, is.readSkip(iboSize), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  if (nFrames != 0) {
    if (shader.hasVTF) {
#ifndef OZ_GL_ES
      int vertexBufferSize = nFramePositions * nFrames * sizeof(float[3]);
      int normalBufferSize = nFramePositions * nFrames * sizeof(float[3]);

      glGenTextures(1, &animationTexId);
      glBindTexture(GL_TEXTURE_2D, animationTexId);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, nFramePositions, 2 * nFrames, 0, GL_RGB,
                   GL_FLOAT, is.readSkip(vertexBufferSize + normalBufferSize));

      glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

      OZ_GL_CHECK_ERROR();
#endif
    }
    else if (nVertices > vertexAnimBufferLength) {
      delete[] vertexAnimBuffer;

      vertexAnimBuffer = new Vertex[nVertices];
      vertexAnimBufferLength = nVertices;
    }
  }

  loadedModels.include(Ref{this});

  delete preloadData;
  preloadData = nullptr;

  OZ_GL_CHECK_ERROR();
}

void Model::unload()
{
  OZ_NACL_IS_MAIN(true);

  if (preloadData != nullptr) {
    delete preloadData;
    preloadData = nullptr;
  }

  if (vbo == 0) {
    return;
  }

  for (const Texture& texture : textures) {
    switch (texture.id) {
      case Texture::NONE: {
        break;
      }
      case Texture::EXTERNAL: {
        context.unloadTexture(&texture);
        break;
      }
      default: {
        context.releaseTexture(texture.id);
        break;
      }
    }
  }

  nodes.clear();
  nodes.trim();
  meshes.clear();
  meshes.trim();
  textures.clear();
  textures.trim();

  if (nFrames != 0) {
    if (shader.hasVTF) {
      glDeleteTextures(1, &animationTexId);
    }
    else {
      delete[] normals;
      delete[] positions;
      delete[] vertices;
    }
  }

  // HACK Crashes during level unloading on NaCl.
#ifndef __native_client__
  glDeleteBuffers(1, &ibo);
  glDeleteBuffers(1, &vbo);
#endif

  ibo = 0;
  vbo = 0;

  loadedModels.exclude(Ref{this});

  OZ_GL_CHECK_ERROR();
}

}
