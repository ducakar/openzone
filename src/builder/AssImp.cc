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
 * @file builder/AssImp.cc
 */

#include <builder/AssImp.hh>

#include <client/Context.hh>
#include <builder/Compiler.hh>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace oz
{
namespace builder
{

struct Anim
{
  enum Behaviour
  {
    DEFAULT,
    NEAREST,
    LINEAR
  };

  struct Key
  {
    float time;
    Vec3  scale;
    Quat  rot;
    Vec3  transl;
  };

  int       firstKey;
  int       nKeys;
  Behaviour behaviour;

  Mat4 interpolate(float time) const;
};

static List<Material>   materials;
static List<Anim::Key>  animKeys;
static List<Anim>       anims;
static Assimp::Importer importer;
static const aiScene*   scene;

static void readNode(const aiNode* node)
{
  if (String::equals(node->mName.C_Str(), "Armature")) {
    return;
  }

  Mat4 transf = ~Mat4(node->mTransformation[0]);

  Log::print("+ %s ", node->mName.C_Str());
  Log() << transf;
  Log::indent();

  compiler.beginNode(node->mName.C_Str());
  compiler.transform(transf);

  if (node->mNumMeshes != 0) {
    compiler.bindMesh(node->mMeshes[0]);
    Log::println("- mesh %u", node->mMeshes[0]);

    if (node->mNumMeshes > 1) {
      for (uint i = 1; i < node->mNumMeshes; ++i) {
        compiler.beginNode();
        compiler.bindMesh(node->mMeshes[i]);
        compiler.endNode();
        Log::println("- mesh %u", node->mMeshes[i]);
      }
    }
  }

  for (uint i = 0; i < scene->mNumLights; ++i) {
    aiLight* light = scene->mLights[i];

    if (String::equals(light->mName.C_Str(), node->mName.C_Str())) {
      compiler.bindLight(i);
      Log::println("- light");
    }
  }

  for (uint i = 0; i < node->mNumChildren; ++i) {
    readNode(node->mChildren[i]);
  }

  compiler.endNode();

  Log::unindent();
}

void AssImp::build(const File& path)
{
  Log::println("Prebuilding Collada model '%s' {", path.c());
  Log::indent();

  File modelFile = path / "data.obj";
  File outFile   = path.toNative() / "data.ozcModel";

  if (!modelFile.exists()) {
    modelFile = path / "data.dae";
  }

  Stream is = modelFile.inputStream();

  if (is.available() == 0) {
    OZ_ERROR("Failed to read '%s' (.dae and .obj extensions probed)", path.c());
  }

  scene = importer.ReadFile(modelFile.realPath(),
                            aiProcess_JoinIdenticalVertices |
                            aiProcess_GenNormals |
                            aiProcess_ValidateDataStructure |
                            aiProcess_ImproveCacheLocality |
                            aiProcess_RemoveRedundantMaterials |
                            aiProcess_FindInvalidData |
                            aiProcess_FindInstances |
                            aiProcess_OptimizeMeshes);
  if (scene == nullptr) {
    OZ_ERROR("Error loading '%s': %s", modelFile.c(), importer.GetErrorString());
  }
  if (!scene->HasMeshes()) {
    OZ_ERROR("Error loading '%s': Meshes missing", modelFile.c());
  }
  if (!scene->HasMaterials()) {
    OZ_ERROR("Error loading '%s': Materials missing", modelFile.c());
  }

  compiler.beginModel();

  for (uint i = 0; i < scene->mNumMeshes; ++i) {
    const aiMesh*     mesh      = scene->mMeshes[i];
    const aiMaterial* material  = scene->mMaterials[mesh->mMaterialIndex];
    const aiVector3D* positions = mesh->mVertices;
    const aiVector3D* normals   = mesh->HasNormals() ? mesh->mNormals : nullptr;
    const aiVector3D* texCoords = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0] : nullptr;

    aiString textureName;
    if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
      material->GetTexture(aiTextureType_DIFFUSE, 0, &textureName);
    }

    String texturePath = "";
    if (textureName.length != 0) {
      texturePath = path / File(textureName.C_Str()).baseName();
    }

    float shininess = 50.0f;
    material->Get(AI_MATKEY_SHININESS, shininess);

    float alpha = 1.0f;
    material->Get(AI_MATKEY_OPACITY, alpha);

    compiler.beginMesh();
    compiler.texture(texturePath);
    compiler.shininess(shininess);
    compiler.blend(alpha != 1.0f);

    for (uint j = 0; j < mesh->mNumFaces; ++j) {
      const aiFace& face = mesh->mFaces[j];

      compiler.begin(Compiler::POLYGON);

      for (uint k = 0; k < face.mNumIndices; ++k) {
        uint index = face.mIndices[k];

        if (texCoords != nullptr) {
          compiler.texCoord(texCoords[index].x, 1.0f - texCoords[index].y);
        }
        if (normals != nullptr) {
          Vec3 normal = Vec3(&normals[index].x);

          if (!Math::isFinite(normal.x) || !Math::isFinite(normal.y) || !Math::isFinite(normal.z)) {
            normal = Vec3::ZERO;
          }

          compiler.normal(normal);
        }
        compiler.vertex(positions[index].x, positions[index].y, positions[index].z);
      }

      compiler.end();
    }

    for (uint j = 0; j < mesh->mNumBones; ++j) {
      Log() << "bone " << mesh->mBones[j]->mName.C_Str()
            << ", " << mesh->mBones[j]->mNumWeights
            << ", " << ~Mat4(mesh->mBones[j]->mOffsetMatrix[0]);
    }

    compiler.endMesh();
  }

  for (uint i = 0; i < scene->mNumLights; ++i) {
    const aiLight* light = scene->mLights[i];

    compiler.beginLight(client::Light::Type(light->mType - 1));

    compiler.position(light->mPosition.x, light->mPosition.y, light->mPosition.z);
    compiler.direction(light->mDirection.x, light->mDirection.y, light->mDirection.z);
    compiler.colour(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b);
    compiler.attenuation(light->mAttenuationConstant, light->mAttenuationLinear,
                         light->mAttenuationQuadratic);
    compiler.coneAngles(light->mAngleInnerCone, light->mAngleOuterCone);

    compiler.endLight();
  }

  for (uint i = 0; i < scene->mRootNode->mNumChildren; ++i) {
    readNode(scene->mRootNode->mChildren[i]);
  }

  for (uint i = 0; i < scene->mNumAnimations; ++i) {
    const aiAnimation* anim = scene->mAnimations[i];

    compiler.beginAnimation();

    for (uint j = 0; j < anim->mNumChannels; ++j) {
      const aiNodeAnim* nodeAnim = anim->mChannels[j];

      compiler.beginChannel();

      Log() << "  positions";
      for (uint k = 0; k < nodeAnim->mNumPositionKeys; ++k) {
        const aiVectorKey& key = nodeAnim->mPositionKeys[k];

        Point position = Point(key.mValue.x, key.mValue.y, key.mValue.z);
        float time     = float(key.mTime);

        Log() << "    " << position << " @ " << time;

        compiler.positionKey(position, time);
      }

      Log() << "  rotations";
      for (uint k = 0; k < nodeAnim->mNumRotationKeys; ++k) {
        const aiQuatKey& key = nodeAnim->mRotationKeys[k];

        Quat  rotation = Quat(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w);
        float time     = float(key.mTime);

        Log() << "    " << rotation << " @ " << time;

        compiler.rotationKey(rotation, time);
      }

      Log() << "  scalings";
      for (uint k = 0; k < nodeAnim->mNumScalingKeys; ++k) {
        const aiVectorKey& key = nodeAnim->mScalingKeys[k];

        Vec3  scaling = Vec3(key.mValue.x, key.mValue.y, key.mValue.z);
        float time    = float(key.mTime);

        Log() << "    " << scaling << " @ " << time;

        compiler.scalingKey(scaling, time);
      }

      compiler.endChannel();
    }

    compiler.endAnimation();
  }

  compiler.endModel();

  scene = nullptr;

  Stream os(0, Endian::LITTLE);

  compiler.writeModel(&os);
  compiler.buildModelTextures(outFile.directory());

  Log::print("Writing to '%s' ...", outFile.c());

  if (!outFile.write(os.begin(), os.tell())) {
    OZ_ERROR("Failed to write %s", outFile.c());
  }

  Log::printEnd(" OK");

  Log::unindent();
  Log::println("}");
}

AssImp assImp;

}
}
