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

#include <client/Terra.hh>

#include <client/Frustum.hh>
#include <client/Camera.hh>
#include <client/Context.hh>

namespace oz::client
{

void Terra::draw()
{
  if (id == -1) {
    return;
  }

  // to match strip triangles with matrix terrain we have to make them clockwise since
  // we draw column-major (triangle strips along y axis) for better cache performance
  glFrontFace(GL_CW);

  span.minX = max(int((camera.p.x - frustum.radius() + oz::Terra::DIM) / TILE_SIZE), 0);
  span.minY = max(int((camera.p.y - frustum.radius() + oz::Terra::DIM) / TILE_SIZE), 0);
  span.maxX = min(int((camera.p.x + frustum.radius() + oz::Terra::DIM) / TILE_SIZE), TILES - 1);
  span.maxY = min(int((camera.p.y + frustum.radius() + oz::Terra::DIM) / TILE_SIZE), TILES - 1);

  shader.program(landShaderId);

  tf.model = Mat4::ID;
  tf.apply();
  tf.applyColour();

  glBindTexture(GL_TEXTURE_2D, detailTex.albedo);
  glActiveTexture(Shader::MASKS);
  glBindTexture(GL_TEXTURE_2D, mapTex);
  if (shader.doBumpMap) {
    glActiveTexture(Shader::NORMALS);
    glBindTexture(GL_TEXTURE_2D, detailTex.normals);
  }

  OZ_GL_CHECK_ERROR();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  for (int i = span.minX; i <= span.maxX; ++i) {
    for (int j = span.minY; j <= span.maxY; ++j) {
      glBindBuffer(GL_ARRAY_BUFFER, vbos[i][j]);

      Vertex::setFormat();

      glDrawElements(GL_TRIANGLE_STRIP, TILE_INDICES, GL_UNSIGNED_SHORT, nullptr);
    }
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  if (shader.doBumpMap) {
    glBindTexture(GL_TEXTURE_2D, shader.defaultNormals);
    glActiveTexture(Shader::MASKS);
  }
  glBindTexture(GL_TEXTURE_2D, shader.defaultMasks);
  glActiveTexture(Shader::DIFFUSE);
  glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

  glFrontFace(GL_CCW);

  OZ_GL_CHECK_ERROR();
}

void Terra::drawLiquid()
{
  if (id == -1) {
    return;
  }

  if (camera.p.z >= 0.0f) {
    glFrontFace(GL_CW);
  }

  waveBias = Math::fmod(waveBias + WAVE_BIAS_INC * Timer::TICK_TIME, Math::TAU);

  shader.program(liquidShaderId);

  tf.model = Mat4::ID;
  tf.apply();
  tf.applyColour();

  glUniform1f(uniform.shininess, 50.0f);
  glUniform1f(uniform.waveBias, waveBias);

  glBindTexture(GL_TEXTURE_2D, liquidTex.albedo);
  glActiveTexture(Shader::MASKS);
  glBindTexture(GL_TEXTURE_2D, liquidTex.masks);
  if (shader.doBumpMap) {
    glActiveTexture(Shader::NORMALS);
    glBindTexture(GL_TEXTURE_2D, liquidTex.normals);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  for (int i = span.minX; i <= span.maxX; ++i) {
    for (int j = span.minY; j <= span.maxY; ++j) {
      if (liquidTiles.get(i * TILES + j)) {
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i][j]);

        Vertex::setFormat();

        glDrawElements(GL_TRIANGLE_STRIP, TILE_INDICES, GL_UNSIGNED_SHORT, nullptr);
      }
    }
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  if (shader.doBumpMap) {
    glBindTexture(GL_TEXTURE_2D, shader.defaultNormals);
    glActiveTexture(Shader::MASKS);
  }
  glBindTexture(GL_TEXTURE_2D, shader.defaultMasks);
  glActiveTexture(Shader::DIFFUSE);
  glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

  if (camera.p.z >= 0.0f) {
    glFrontFace(GL_CCW);
  }

  OZ_GL_CHECK_ERROR();
}

void Terra::load()
{
  OZ_NACL_IS_MAIN(true);

  id = orbis.terra.id;

  if (id == -1) {
    return;
  }

  const String& name = liber.terrae[id].name;

  File   file = "@terra/" + name + ".ozcTerra";
  File   map  = "@terra/" + name + ".dds";
  Stream is(0, Endian::LITTLE);

  if (!file.read(&is)) {
    OZ_ERROR("Terra file '%s' read failed", file.c());
  }

  glGenBuffers(TILES * TILES, &vbos[0][0]);
  glGenBuffers(1, &ibo);

  int vboSize = TILE_VERTICES * sizeof(Vertex);
  int iboSize = TILE_INDICES  * sizeof(uint16);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboSize, is.readSkip(iboSize), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  for (int i = 0; i < TILES; ++i) {
    for (int j = 0; j < TILES; ++j) {
      Vertex* vertices = new Vertex[TILE_VERTICES];

      for (int k = 0; k <= TILE_QUADS; ++k) {
        for (int l = 0; l <= TILE_QUADS; ++l) {
          int x = i * TILE_QUADS + k;
          int y = j * TILE_QUADS + l;

          Vertex& vertex = vertices[k * (TILE_QUADS + 1) + l];

          vertex.pos[0]      = orbis.terra.quads[x][y].vertex.x;
          vertex.pos[1]      = orbis.terra.quads[x][y].vertex.y;
          vertex.pos[2]      = orbis.terra.quads[x][y].vertex.z;

          vertex.texCoord[0] = int16(x);
          vertex.texCoord[1] = int16(oz::Terra::VERTS - y);

          vertex.normal[0]   = is.readByte();
          vertex.normal[1]   = is.readByte();
          vertex.normal[2]   = is.readByte();

          vertex.tangent[0]  = byte(127);
          vertex.tangent[1]  = 0;
          vertex.tangent[2]  = 0;

          vertex.binormal[0] = 0;
          vertex.binormal[1] = byte(127);
          vertex.binormal[2] = 0;

          vertex.bone[0]     = 0;
          vertex.bone[1]     = 0;

          vertex.weight[0]   = 0;
          vertex.weight[0]   = 0;
        }
      }

      glBindBuffer(GL_ARRAY_BUFFER, vbos[i][j]);
      glBufferData(GL_ARRAY_BUFFER, vboSize, vertices, GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      delete[] vertices;
    }
  }

  is.readBitset(liquidTiles);

  detailTexId     = liber.textureIndex(is.readString());
  liquidTexId     = liber.textureIndex(is.readString());
  liquidFogColour = is.read<Vec4>();

  detailTex       = context.requestTexture(detailTexId);
  liquidTex       = context.requestTexture(liquidTexId);

  glGenTextures(1, &mapTex);
  glBindTexture(GL_TEXTURE_2D, mapTex);

  if (GL::textureDataFromFile(map, context.textureLod) == 0) {
    OZ_ERROR("Failed to load terain map texture '%s'", map.c());
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

  landShaderId   = liber.shaderIndex("terraLand");
  liquidShaderId = liber.shaderIndex("terraLiquid");

  OZ_ASSERT(is.available() == 0);
}

void Terra::unload()
{
  OZ_NACL_IS_MAIN(true);

  if (id == -1) {
    return;
  }

  context.releaseTexture(detailTexId);
  context.releaseTexture(liquidTexId);

  glDeleteTextures(1, &mapTex);

  glDeleteBuffers(1, &ibo);
  glDeleteBuffers(TILES * TILES, &vbos[0][0]);

  ibo = 0;
  for (auto& column : vbos) {
    for (uint& quad : column) {
      quad = 0;
    }
  }

  id = -1;
}

Terra terra;

}
