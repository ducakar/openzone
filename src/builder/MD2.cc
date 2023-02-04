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

#include <builder/MD2.hh>

#include <client/MD2.hh>
#include <client/Context.hh>
#include <builder/Compiler.hh>

namespace oz::builder
{

static const Vec3 NORMALS[] =
{
  Vec3(-0.000000f, -0.525731f,  0.850651f),
  Vec3(-0.238856f, -0.442863f,  0.864188f),
  Vec3(-0.000000f, -0.295242f,  0.955423f),
  Vec3(-0.500000f, -0.309017f,  0.809017f),
  Vec3(-0.262866f, -0.162460f,  0.951056f),
  Vec3(-0.000000f,  0.000000f,  1.000000f),
  Vec3(-0.850651f,  0.000000f,  0.525731f),
  Vec3(-0.716567f, -0.147621f,  0.681718f),
  Vec3(-0.716567f,  0.147621f,  0.681718f),
  Vec3(-0.525731f,  0.000000f,  0.850651f),
  Vec3(-0.500000f,  0.309017f,  0.809017f),
  Vec3(-0.000000f,  0.525731f,  0.850651f),
  Vec3(-0.000000f,  0.295242f,  0.955423f),
  Vec3(-0.238856f,  0.442863f,  0.864188f),
  Vec3(-0.262866f,  0.162460f,  0.951056f),
  Vec3(-0.147621f, -0.681718f,  0.716567f),
  Vec3(-0.309017f, -0.809017f,  0.500000f),
  Vec3(-0.425325f, -0.587785f,  0.688191f),
  Vec3(-0.525731f, -0.850651f,  0.000000f),
  Vec3(-0.442863f, -0.864188f,  0.238856f),
  Vec3(-0.681718f, -0.716567f,  0.147621f),
  Vec3(-0.587785f, -0.688191f,  0.425325f),
  Vec3(-0.809017f, -0.500000f,  0.309017f),
  Vec3(-0.864188f, -0.238856f,  0.442863f),
  Vec3(-0.688191f, -0.425325f,  0.587785f),
  Vec3(-0.681718f, -0.716567f, -0.147621f),
  Vec3(-0.809017f, -0.500000f, -0.309017f),
  Vec3(-0.850651f, -0.525731f,  0.000000f),
  Vec3(-0.850651f,  0.000000f, -0.525731f),
  Vec3(-0.864188f, -0.238856f, -0.442863f),
  Vec3(-0.955423f,  0.000000f, -0.295242f),
  Vec3(-0.951056f, -0.262866f, -0.162460f),
  Vec3(-1.000000f,  0.000000f,  0.000000f),
  Vec3(-0.955423f,  0.000000f,  0.295242f),
  Vec3(-0.951056f, -0.262866f,  0.162460f),
  Vec3(-0.864188f,  0.238856f,  0.442863f),
  Vec3(-0.951056f,  0.262866f,  0.162460f),
  Vec3(-0.809017f,  0.500000f,  0.309017f),
  Vec3(-0.864188f,  0.238856f, -0.442863f),
  Vec3(-0.951056f,  0.262866f, -0.162460f),
  Vec3(-0.809017f,  0.500000f, -0.309017f),
  Vec3(-0.525731f,  0.850651f,  0.000000f),
  Vec3(-0.681718f,  0.716567f,  0.147621f),
  Vec3(-0.681718f,  0.716567f, -0.147621f),
  Vec3(-0.850651f,  0.525731f,  0.000000f),
  Vec3(-0.688191f,  0.425325f,  0.587785f),
  Vec3(-0.442863f,  0.864188f,  0.238856f),
  Vec3(-0.587785f,  0.688191f,  0.425325f),
  Vec3(-0.309017f,  0.809017f,  0.500000f),
  Vec3(-0.147621f,  0.681718f,  0.716567f),
  Vec3(-0.425325f,  0.587785f,  0.688191f),
  Vec3(-0.295242f,  0.955423f,  0.000000f),
  Vec3(-0.000000f,  1.000000f,  0.000000f),
  Vec3(-0.162460f,  0.951056f,  0.262866f),
  Vec3( 0.525731f,  0.850651f,  0.000000f),
  Vec3( 0.295242f,  0.955423f,  0.000000f),
  Vec3( 0.442863f,  0.864188f,  0.238856f),
  Vec3( 0.162460f,  0.951056f,  0.262866f),
  Vec3( 0.309017f,  0.809017f,  0.500000f),
  Vec3( 0.147621f,  0.681718f,  0.716567f),
  Vec3(-0.000000f,  0.850651f,  0.525731f),
  Vec3(-0.442863f,  0.864188f, -0.238856f),
  Vec3(-0.309017f,  0.809017f, -0.500000f),
  Vec3(-0.162460f,  0.951056f, -0.262866f),
  Vec3(-0.000000f,  0.525731f, -0.850651f),
  Vec3(-0.147621f,  0.681718f, -0.716567f),
  Vec3( 0.147621f,  0.681718f, -0.716567f),
  Vec3(-0.000000f,  0.850651f, -0.525731f),
  Vec3( 0.309017f,  0.809017f, -0.500000f),
  Vec3( 0.442863f,  0.864188f, -0.238856f),
  Vec3( 0.162460f,  0.951056f, -0.262866f),
  Vec3(-0.716567f,  0.147621f, -0.681718f),
  Vec3(-0.500000f,  0.309017f, -0.809017f),
  Vec3(-0.688191f,  0.425325f, -0.587785f),
  Vec3(-0.238856f,  0.442863f, -0.864188f),
  Vec3(-0.425325f,  0.587785f, -0.688191f),
  Vec3(-0.587785f,  0.688191f, -0.425325f),
  Vec3(-0.716567f, -0.147621f, -0.681718f),
  Vec3(-0.500000f, -0.309017f, -0.809017f),
  Vec3(-0.525731f,  0.000000f, -0.850651f),
  Vec3(-0.000000f, -0.525731f, -0.850651f),
  Vec3(-0.238856f, -0.442863f, -0.864188f),
  Vec3(-0.000000f, -0.295242f, -0.955423f),
  Vec3(-0.262866f, -0.162460f, -0.951056f),
  Vec3(-0.000000f,  0.000000f, -1.000000f),
  Vec3(-0.000000f,  0.295242f, -0.955423f),
  Vec3(-0.262866f,  0.162460f, -0.951056f),
  Vec3( 0.238856f, -0.442863f, -0.864188f),
  Vec3( 0.500000f, -0.309017f, -0.809017f),
  Vec3( 0.262866f, -0.162460f, -0.951056f),
  Vec3( 0.850651f,  0.000000f, -0.525731f),
  Vec3( 0.716567f, -0.147621f, -0.681718f),
  Vec3( 0.716567f,  0.147621f, -0.681718f),
  Vec3( 0.525731f,  0.000000f, -0.850651f),
  Vec3( 0.500000f,  0.309017f, -0.809017f),
  Vec3( 0.238856f,  0.442863f, -0.864188f),
  Vec3( 0.262866f,  0.162460f, -0.951056f),
  Vec3( 0.864188f,  0.238856f, -0.442863f),
  Vec3( 0.809017f,  0.500000f, -0.309017f),
  Vec3( 0.688191f,  0.425325f, -0.587785f),
  Vec3( 0.681718f,  0.716567f, -0.147621f),
  Vec3( 0.587785f,  0.688191f, -0.425325f),
  Vec3( 0.425325f,  0.587785f, -0.688191f),
  Vec3( 0.955423f,  0.000000f, -0.295242f),
  Vec3( 1.000000f,  0.000000f,  0.000000f),
  Vec3( 0.951056f,  0.262866f, -0.162460f),
  Vec3( 0.850651f,  0.000000f,  0.525731f),
  Vec3( 0.955423f,  0.000000f,  0.295242f),
  Vec3( 0.864188f,  0.238856f,  0.442863f),
  Vec3( 0.951056f,  0.262866f,  0.162460f),
  Vec3( 0.809017f,  0.500000f,  0.309017f),
  Vec3( 0.681718f,  0.716567f,  0.147621f),
  Vec3( 0.850651f,  0.525731f,  0.000000f),
  Vec3( 0.864188f, -0.238856f, -0.442863f),
  Vec3( 0.809017f, -0.500000f, -0.309017f),
  Vec3( 0.951056f, -0.262866f, -0.162460f),
  Vec3( 0.525731f, -0.850651f,  0.000000f),
  Vec3( 0.681718f, -0.716567f, -0.147621f),
  Vec3( 0.681718f, -0.716567f,  0.147621f),
  Vec3( 0.850651f, -0.525731f,  0.000000f),
  Vec3( 0.809017f, -0.500000f,  0.309017f),
  Vec3( 0.864188f, -0.238856f,  0.442863f),
  Vec3( 0.951056f, -0.262866f,  0.162460f),
  Vec3( 0.442863f, -0.864188f,  0.238856f),
  Vec3( 0.309017f, -0.809017f,  0.500000f),
  Vec3( 0.587785f, -0.688191f,  0.425325f),
  Vec3( 0.147621f, -0.681718f,  0.716567f),
  Vec3( 0.238856f, -0.442863f,  0.864188f),
  Vec3( 0.425325f, -0.587785f,  0.688191f),
  Vec3( 0.500000f, -0.309017f,  0.809017f),
  Vec3( 0.716567f, -0.147621f,  0.681718f),
  Vec3( 0.688191f, -0.425325f,  0.587785f),
  Vec3( 0.262866f, -0.162460f,  0.951056f),
  Vec3( 0.238856f,  0.442863f,  0.864188f),
  Vec3( 0.262866f,  0.162460f,  0.951056f),
  Vec3( 0.500000f,  0.309017f,  0.809017f),
  Vec3( 0.716567f,  0.147621f,  0.681718f),
  Vec3( 0.525731f,  0.000000f,  0.850651f),
  Vec3( 0.688191f,  0.425325f,  0.587785f),
  Vec3( 0.425325f,  0.587785f,  0.688191f),
  Vec3( 0.587785f,  0.688191f,  0.425325f),
  Vec3(-0.295242f, -0.955423f,  0.000000f),
  Vec3(-0.162460f, -0.951056f,  0.262866f),
  Vec3(-0.000000f, -1.000000f,  0.000000f),
  Vec3(-0.000000f, -0.850651f,  0.525731f),
  Vec3( 0.295242f, -0.955423f,  0.000000f),
  Vec3( 0.162460f, -0.951056f,  0.262866f),
  Vec3(-0.442863f, -0.864188f, -0.238856f),
  Vec3(-0.162460f, -0.951056f, -0.262866f),
  Vec3(-0.309017f, -0.809017f, -0.500000f),
  Vec3( 0.442863f, -0.864188f, -0.238856f),
  Vec3( 0.162460f, -0.951056f, -0.262866f),
  Vec3( 0.309017f, -0.809017f, -0.500000f),
  Vec3(-0.147621f, -0.681718f, -0.716567f),
  Vec3( 0.147621f, -0.681718f, -0.716567f),
  Vec3(-0.000000f, -0.850651f, -0.525731f),
  Vec3(-0.587785f, -0.688191f, -0.425325f),
  Vec3(-0.425325f, -0.587785f, -0.688191f),
  Vec3(-0.688191f, -0.425325f, -0.587785f),
  Vec3( 0.688191f, -0.425325f, -0.587785f),
  Vec3( 0.425325f, -0.587785f, -0.688191f),
  Vec3( 0.587785f, -0.688191f, -0.425325f)
};

void MD2::build(const File& path)
{
  File modelFile  = path / "tris.md2";
  File configFile = path / "config.json";
  File skinPath   = path / "skin";

  Log::println("Prebuilding MD2 model '%s' {", path.c());
  Log::indent();

  Json config;
  config.load(configFile);

  Stream is(0, Endian::LITTLE);
  if (!modelFile.read(&is)) {
    OZ_ERROR("MD2 file read failed");
  }

  MD2Header header;

  header.id[0]   = is.readChar();
  header.id[1]   = is.readChar();
  header.id[2]   = is.readChar();
  header.id[3]   = is.readChar();
  header.version = is.readInt();

  if (header.id[0] != 'I' || header.id[1] != 'D' || header.id[2] != 'P' || header.id[3] != '2' ||
      header.version != 8)
  {
    OZ_ERROR("Wrong Quake 2 MD2 format");
  }

  header.skinWidth  = is.readInt();
  header.skinHeight = is.readInt();
  header.frameSize  = is.readInt();

  // MD2Header::nSkins
  is.readInt();

  header.nFramePositions = is.readInt();
  header.nTexCoords      = is.readInt();
  header.nTriangles      = is.readInt();

  // MD2Header::nGLCmds
  is.readInt();

  header.nFrames = is.readInt();

  // MD2Header::offSkins
  is.readInt();

  header.offTexCoords = is.readInt();
  header.offTriangles = is.readInt();
  header.offFrames    = is.readInt();

  // MD2Header::offGLCmds
  is.readInt();
  // MD2Header::offEnd
  is.readInt();

  if (header.nFrames <= 0 || header.nFramePositions <= 0) {
    OZ_ERROR("MD2 model loading error");
  }

  if (config["forceStatic"].get(false)) {
    header.nFrames = 1;
  }
  String shaderName      = config["shader"].get(header.nFrames == 1 ? "mesh" : "dmesh");
  float  shininess       = config["shininess"].get(50.0f);
  float  scale           = config["scale"].get(2.0f / 48.0f);
  Vec3   translation     = config["translate"].get(Vec3::ZERO);
  Vec3   jumpTranslation = config["jumpTranslate"].get(Vec3::ZERO);
  Vec3   freeTranslation = config["weaponTranslate"].get(Vec3::ZERO);
  Vec3   freeRotation    = config["weaponRotate"].get(Vec3::ZERO);

  config.clear(true);

  Mat4 freeTransf = Mat4::ID;
  freeTransf.translate(freeTranslation);
  freeTransf.rotateY(Math::rad(freeRotation.y));
  freeTransf.rotateX(Math::rad(freeRotation.x));
  freeTransf.rotateZ(Math::rad(freeRotation.z));

  List<client::TexCoord> texCoords(header.nTexCoords);
  List<MD2Triangle>      triangles(header.nTriangles);
  List<Vec3>             normals(header.nFrames * header.nFramePositions);
  List<Point>            positions(header.nFrames * header.nFramePositions);

  is.rewind();
  is.readSkip(header.offFrames);

  const char* frameData = is.readSkip(header.nFrames * header.frameSize);

  for (int i = 0; i < header.nFrames; ++i) {
    const MD2Frame& frame = *reinterpret_cast<const MD2Frame*>(&frameData[i * header.frameSize]);

    for (int j = 0; j < header.nFramePositions; ++j) {
      Vec3&  normal   = normals[i * header.nFramePositions + j];
      Point& position = positions[i * header.nFramePositions + j];

      normal = NORMALS[frame.verts[j].normal];

      position.x = float(frame.verts[j].p[1]) * -frame.scale[1] - frame.translate[1];
      position.y = float(frame.verts[j].p[0]) *  frame.scale[0] + frame.translate[0];
      position.z = float(frame.verts[j].p[2]) *  frame.scale[2] + frame.translate[2];

      position.x = position.x * scale + translation.x;
      position.y = position.y * scale + translation.y;
      position.z = position.z * scale + translation.z;

      if (client::MD2::ANIM_LIST[client::MD2::ANIM_JUMP].firstFrame <= i &&
          i <= client::MD2::ANIM_LIST[client::MD2::ANIM_JUMP].lastFrame)
      {
        position += jumpTranslation;
      }
    }
  }

  is.rewind();
  is.readSkip(header.offTexCoords);

  for (TexCoord& texCoord : texCoords) {
    texCoord.u = float(is.readInt16()) / float(header.skinWidth);
    texCoord.v = float(is.readInt16()) / float(header.skinHeight);
  }

  is.rewind();
  is.readSkip(header.offTriangles);

  for (MD2Triangle& triangle : triangles) {
    triangle.vertices[0]  = is.readInt16();
    triangle.vertices[1]  = is.readInt16();
    triangle.vertices[2]  = is.readInt16();
    triangle.texCoords[0] = is.readInt16();
    triangle.texCoords[1] = is.readInt16();
    triangle.texCoords[2] = is.readInt16();
  }

  compiler.beginModel();

  compiler.enable(Compiler::UNIQUE);
  compiler.enable(Compiler::CLOCKWISE);
  compiler.shader(shaderName);

  if (header.nFrames != 1) {
    compiler.anim(header.nFrames, header.nFramePositions);
    compiler.animPositions(positions[0]);
    compiler.animNormals(normals[0]);
  }

  compiler.beginMesh();
  compiler.texture(skinPath);
  compiler.shininess(shininess);
  compiler.begin(Compiler::TRIANGLES);

  for (int i = 0; i < header.nTriangles; ++i) {
    for (int j = 0; j < 3; ++j) {
      compiler.texCoord(texCoords[triangles[i].texCoords[j]]);

      if (header.nFrames == 1) {
        compiler.normal(normals[triangles[i].vertices[j]]);
        compiler.vertex(positions[triangles[i].vertices[j]]);
      }
      else {
        // vertex index in vertex animation buffer
        compiler.animVertex(triangles[i].vertices[j]);
      }
    }
  }

  compiler.end();
  compiler.endMesh();

  compiler.beginNode();
  compiler.transform(freeTransf);
  compiler.bindMesh(0);
  compiler.endNode();

  compiler.beginNode();
  compiler.includeInBounds(false);
  compiler.bindMesh(0);
  compiler.endNode();

  compiler.endModel();

  texCoords.clear();
  texCoords.trim();
  triangles.clear();
  triangles.trim();
  normals.clear();
  normals.trim();
  positions.clear();
  positions.trim();

  File destDir = path.toNative();
  destDir.mkdir();

  Stream os(0, Endian::LITTLE);

  compiler.writeModel(&os);
  compiler.buildModelTextures(destDir);

  File destFile = destDir / "data.ozcModel";

  Log::print("Writing to '%s' ...", destFile.c());

  if (!destFile.write(os)) {
    OZ_ERROR("Failed to write '%s'", destFile.c());
  }

  Log::printEnd(" OK");

  Log::unindent();
  Log::println("}");
}

MD2 md2;

}
