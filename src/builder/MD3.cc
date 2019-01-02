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

#include <builder/MD3.hh>

#include <client/Context.hh>
#include <builder/Compiler.hh>

namespace oz::builder
{

void MD3::readAnimData()
{
  File animFile = dir / "animation.cfg";

  String realPath = animFile.realDirectory() + "/" + animFile;

  FILE* fs = fopen(realPath, "r");
  if (fs == nullptr) {
    OZ_ERROR("Reading animation data failed");
  }

  char line[1024];

  while (fgets(line, 1024, fs) != nullptr) {
  }

  fclose(fs);
}

void MD3::buildMesh(const char* name, int meshFrame)
{
  Log::print("Mesh '%s' ...", name);

  File   file = String::format("%s/%s.md3", dir.c(), name);
  Stream is(0, Endian::LITTLE);

  if (!file.read(&is)) {
    OZ_ERROR("Cannot read MD3 model part file '%s'", file.c());
  }

  MD3Header header;

  header.id[0]   = is.readChar();
  header.id[1]   = is.readChar();
  header.id[2]   = is.readChar();
  header.id[3]   = is.readChar();
  header.version = is.readInt();

  if (header.id[0] != 'I' || header.id[1] != 'D' || header.id[2] != 'P' || header.id[3] != '3') {
    OZ_ERROR("MD3 model part file has an invalid format");
  }

  // header.fileName
  is.readSkip(64);

  header.flags       = is.readInt();
  header.nFrames     = is.readInt();
  header.nTags       = is.readInt();
  header.nSurfaces   = is.readInt();
  header.nSkins      = is.readInt();
  header.offFrames   = is.readInt();
  header.offTags     = is.readInt();
  header.offSurfaces = is.readInt();
  header.offEnd      = is.readInt();

  if (header.nFrames == 0 || header.nSurfaces == 0) {
    OZ_ERROR("Invalid MD3 header counts");
  }

  if (String::equals(name, "lower")) {
    nLowerFrames = header.nFrames;
  }
  else if (String::equals(name, "upper")) {
    nUpperFrames = header.nFrames;
  }

//   if (header.nTags != 0) {
//     is.rewind();
//     is.forward(header.offTags);
//
//     for (int i = 0; i < header.nTags; ++i) {
//       const char* tag = is.forward(64);
//
//       float tx  = is.readFloat();
//       float ty  = is.readFloat();
//       float tz  = is.readFloat();
//
//       float m00 = is.readFloat();
//       float m01 = is.readFloat();
//       float m02 = is.readFloat();
//       float m10 = is.readFloat();
//       float m11 = is.readFloat();
//       float m12 = is.readFloat();
//       float m20 = is.readFloat();
//       float m21 = is.readFloat();
//       float m22 = is.readFloat();
//
//       Vec3 transl = Vec3(scale * -ty, scale * tx, scale * tz);
//       Mat4 rotMat = Mat4(+m11, -m10, -m12, 0.0f,
//                          -m01, +m00, +m02, 0.0f,
//                          -m21, +m20, +m22, 0.0f,
//                          0.0f, 0.0f, 0.0f, 1.0f);
//
//       if (String::equals(name, "lower")) {
//         if (String::equals(name, "tag_torso")) {
//           joints[i][client::MD3::JOINT_HIP].transl = transl;
//           joints[i][client::MD3::JOINT_HIP].rot    = rotMat.toQuat();
//         }
//       }
//       else if (String::equals(name, "upper")) {
//         if (String::equals(tag, "tag_head")) {
//           joints[i][client::MD3::JOINT_NECK].transl = transl;
//           joints[i][client::MD3::JOINT_NECK].rot    = rotMat.toQuat();
//         }
//         else if (String::equals(tag, "tag_weapon")) {
//           joints[i][client::MD3::JOINT_WEAPON].transl = transl;
//           joints[i][client::MD3::JOINT_WEAPON].rot    = rotMat.toQuat();
//         }
//       }
//     }
//   }

  is.rewind();
  is.readSkip(header.offSurfaces);

  for (int i = 0; i < header.nSurfaces; ++i) {
    int surfaceStart = is.tell();

    MD3Surface surface;

    surface.id           = is.readInt();
    Arrays::copy(is.readSkip(64), 64, surface.name);
    surface.flags        = is.readInt();

    surface.nFrames      = is.readInt();
    surface.nShaders     = is.readInt();
    surface.nVertices    = is.readInt();
    surface.nTriangles   = is.readInt();

    surface.offTriangles = is.readInt();
    surface.offShaders   = is.readInt();
    surface.offTexCoords = is.readInt();
    surface.offVertices  = is.readInt();
    surface.offEnd       = is.readInt();

    if (surface.nFrames == 0 || surface.nTriangles == 0 || surface.nShaders == 0 ||
        surface.nVertices == 0)
    {
      OZ_ERROR("Invalid MD3 surface counts");
    }

    if (surface.nFrames != header.nFrames) {
      OZ_ERROR("Invalid MD3 surface # of frames");
    }

    String texture;

    List<MD3Triangle>      surfaceTriangles(surface.nTriangles);
    List<MD3Shader>        surfaceShaders(surface.nShaders);
    List<client::TexCoord> surfaceTexCoords(surface.nVertices);
    List<MD3Vertex>        surfaceVertices(surface.nFrames * surface.nVertices);
    List<Vec3>             normals(surfaceVertices.size());
    List<Point>            vertices(surfaceVertices.size());

    is.rewind();
    is.readSkip(surfaceStart + surface.offTriangles);

    for (int j = 0; j < surfaceTriangles.size(); ++j) {
      surfaceTriangles[j].vertices[0] = is.readInt();
      surfaceTriangles[j].vertices[1] = is.readInt();
      surfaceTriangles[j].vertices[2] = is.readInt();
    }

    is.rewind();
    is.readSkip(surfaceStart + surface.offShaders);

    for (int j = 0; j < surfaceShaders.size(); ++j) {
      Arrays::copy(is.readSkip(64), 64, surfaceShaders[j].name);
      surfaceShaders[j].index = is.readInt();
    }

    if (skin.isEmpty()) {
      File skinFile = String::replace(surfaceShaders[0].name, '\\', '/');
      texture = skinFile.baseName();
    }
    else {
      File skinFile = skin;
      texture = skinFile.baseName();
    }

    is.rewind();
    is.readSkip(surfaceStart + surface.offTexCoords);

    for (int j = 0; j < surfaceTexCoords.size(); ++j) {
      surfaceTexCoords[j].u = is.readFloat();
      surfaceTexCoords[j].v = 1.0f - is.readFloat();
    }

    is.rewind();
    is.readSkip(surfaceStart + surface.offVertices);

    for (int j = 0; j < surfaceVertices.size(); ++j) {
      vertices[j].y = float(+is.readInt16()) / 64.0f * scale;
      vertices[j].x = float(-is.readInt16()) / 64.0f * scale;
      vertices[j].z = float(+is.readInt16()) / 64.0f * scale;

      float h  = float(is.readChar()) / 255.0f * Math::TAU;
      float v  = float(is.readChar()) / 255.0f * Math::TAU;
      float xy = Math::sin(v);

      normals[j].y = +Math::cos(h) * xy;
      normals[j].x = -Math::sin(h) * xy;
      normals[j].z = +Math::cos(v);
    }

    is.rewind();
    is.readSkip(surfaceStart + surface.offEnd);

    compiler.beginMesh();
    compiler.texture(dir / texture);
    compiler.begin(Compiler::TRIANGLES);

    for (int j = 0; j < surfaceTriangles.size(); ++j) {
      for (int k = 0; k < 3; ++k) {
        int l = surfaceTriangles[j].vertices[k];
        int m = meshFrame == -1 ? l : meshFrame * surface.nVertices + l;

        compiler.texCoord(surfaceTexCoords[l]);
        compiler.normal(meshTransf * normals[m]);
        compiler.vertex(meshTransf * vertices[m]);
      }
    }

    compiler.end();
    int meshId = compiler.endMesh();

    compiler.beginNode();
    compiler.bindMesh(meshId);
    compiler.endNode();
  }

  Log::printEnd(" OK");
}

void MD3::load()
{
  File configFile = dir / "config.json";

  Json config;
  config.load(configFile);

  scale      = config["scale"].get(0.04f);
  skin       = config["skin"].get("");

  model      = config["model"].get("");
  frame      = config["frame"].get(-1);
  lowerFrame = config["lowerFrame"].get(-1);
  upperFrame = config["upperFrame"].get(-1);
  shaderName = config["shader"].get(frame == -1 ? "md3" : "mesh");

  Vec3 weaponTranslation = config["weaponTranslate"].get(Vec3::ZERO);
  Vec3 weaponRotation    = config["weaponRotate"].get(Vec3::ZERO);

  Mat4 weaponTransf = Mat4::translation(weaponTranslation);
  weaponTransf.rotateX(Math::rad(weaponRotation.x));
  weaponTransf.rotateZ(Math::rad(weaponRotation.z));
  weaponTransf.rotateY(Math::rad(weaponRotation.y));

  config.clear(true);
}

void MD3::save()
{
  Stream os(0, Endian::LITTLE);

  compiler.beginModel();

  compiler.enable(Compiler::UNIQUE);
  compiler.enable(Compiler::CLOCKWISE);
  compiler.shader(shaderName);

  if (!String::isEmpty(model)) {
    if (frame == -1) {
      OZ_ERROR("Custom models can only be static. Must specify frame");
    }

    meshTransf = Mat4::ID;

    buildMesh(model, frame);
  }
//   else if (frame >= 0) {
//     meshTransf = Mat4::ID;
//
//     buildMesh("lower", frame);
//
//     meshTransf = meshTransf * Mat4::translation(joints[frame][client::MD3::JOINT_HIP].transl);
//     meshTransf = meshTransf * Mat4::rotation(joints[frame][client::MD3::JOINT_HIP].rot);
//
//     buildMesh("upper", frame);
//
//     meshTransf = meshTransf * Mat4::translation(joints[frame][client::MD3::JOINT_NECK].transl);
//     meshTransf = meshTransf * Mat4::rotation(joints[frame][client::MD3::JOINT_NECK].rot);
//
//     buildMesh("head", 0);
//   }
  else {
    buildMesh("lower", frame);
    buildMesh("upper", frame);
    buildMesh("head", 0);

    os.writeInt(nLowerFrames);
    os.writeInt(nUpperFrames);

//     for (int i = 0; i < nLowerFrames; ++i) {
//       os.write<Vec3>(joints[i][client::MD3::JOINT_HIP].transl);
//       os.writeQuat(joints[i][client::MD3::JOINT_HIP].rot);
//     }
//     for (int i = 0; i < nUpperFrames; ++i) {
//       os.write<Vec3>(joints[i][client::MD3::JOINT_NECK].transl);
//       os.writeQuat(joints[i][client::MD3::JOINT_NECK].rot);
//     }
//     for (int i = 0; i < nUpperFrames; ++i) {
//       os.write<Vec3>(joints[i][client::MD3::JOINT_WEAPON].transl);
//       os.writeQuat(joints[i][client::MD3::JOINT_WEAPON].rot);
//     }
  }

  compiler.endModel();

  File destDir = dir.substring(1);
  destDir.mkdir();

  compiler.writeModel(&os);
  compiler.buildModelTextures(destDir);

  if (frame == -1) {
    File destFile = destDir / "data.ozcMD3";

    Log::print("Writing to '%s' ...", destFile.c());

    if (!destFile.write(os)) {
      OZ_ERROR("Failed to write '%s'", destFile.c());
    }

    Log::printEnd(" OK");
  }
  else {
    File destFile = destDir / "data.ozcModel";

    Log::print("Writing to '%s' ...", destFile.c());

    if (!destFile.write(os)) {
      OZ_ERROR("Failed to write '%s'", destFile.c());
    }

    Log::printEnd(" OK");
  }
}

void MD3::build(const File& path)
{
  Log::println("Prebuilding MD3 model '%s' {", path.c());
  Log::indent();

  dir = path;

  load();
  save();

  dir        = "";
  skin       = "";
  masks      = "";
  model      = "";
  shaderName = "";

  Log::unindent();
  Log::println("}");
}

MD3 md3;

}
