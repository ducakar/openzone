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

#include <client/Caelum.hh>

#include <client/Shader.hh>
#include <client/Shape.hh>

namespace oz::client
{

const char* const Caelum::SKYBOX_FACES[] = {"+x", "-x", "+y", "-y", "+z", "-z"};

void Caelum::update()
{
  if (id == -1) {
    return;
  }

  angle = Math::TAU * (orbis.caelum.time / orbis.caelum.period);

  Mat4 rot = Mat4::rotation(Quat::rotationAxis(axis, angle));
  Vec3 dir = rot * originalLightDir;

  ratio = clamp(dir.z + DAY_BIAS, 0.0f, 1.0f);
  float ratioDiff = 1.0f - abs(1.0f - 2.0f * ratio);

  caelumColour.x = Math::mix(NIGHT_COLOUR.x, DAY_COLOUR.x, ratio) + RED_COEF   * ratioDiff;
  caelumColour.y = Math::mix(NIGHT_COLOUR.y, DAY_COLOUR.y, ratio) + GREEN_COEF * ratioDiff;
  caelumColour.z = Math::mix(NIGHT_COLOUR.z, DAY_COLOUR.z, ratio) + BLUE_COEF  * ratioDiff;

  lightDir = dir;

  diffuseColour.x = DIFFUSE_COEF * (ratio + RED_COEF   * ratioDiff);
  diffuseColour.y = DIFFUSE_COEF * (ratio + GREEN_COEF * ratioDiff);
  diffuseColour.z = DIFFUSE_COEF * (ratio + BLUE_COEF  * ratioDiff);

  ambientColour.x = AMBIENT_COEF * diffuseColour.x;
  ambientColour.y = AMBIENT_COEF * diffuseColour.y;
  ambientColour.z = AMBIENT_COEF * diffuseColour.z;
}

void Caelum::draw()
{
  if (id == -1) {
    return;
  }

  OZ_GL_CHECK_ERROR();

  // we need the transformation matrix for occlusion of stars below horizon
  Mat4 transf = Mat4::rotationZ(orbis.caelum.heading) ^
                Mat4::rotationY(angle - Math::TAU / 4.0f);

  shape.bind();
  shader.program(celestialShaderId);

  Vec4 sunColour = Vec4(ambientColour.x + 2.0f * diffuseColour.x,
                        ambientColour.y + diffuseColour.y,
                        ambientColour.z + diffuseColour.z,
                        1.0f);

  sunColour = max(sunColour, caelumColour);

  glUniform3fv(uniform.caelumColour, 1, caelumColour);
  glUniform1f(uniform.caelumLuminance, max(-lightDir.z, 0.0f));

  tf.applyCamera();
  tf.model = transf;
  tf.applyColour();

  shape.skyBox(skyboxTexIds);

  glDisable(GL_CULL_FACE);
  glBindTexture(GL_TEXTURE_2D, sunTexId);

  tf.model = transf;
  tf.model.translate(Vec3(0.0f, 0.0f, +15.0f));
  tf.setColour(tf.colour * sunColour);

  shape.quad(1.0f, 1.0f);

  glBindTexture(GL_TEXTURE_2D, moonTexId);

  tf.model = transf;
  tf.model.translate(Vec3(0.0f, 0.0f, -15.0f));
  tf.setColour(tf.colour);

  shape.quad(1.0f, 1.0f);

  glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);
  glEnable(GL_CULL_FACE);

  shape.unbind();

  OZ_GL_CHECK_ERROR();
}

void Caelum::load()
{
  id = orbis.caelum.id;

  if (id == -1) {
    return;
  }

  const File& path = liber.caela[id].path;

  glGenTextures(1, &sunTexId);
  glBindTexture(GL_TEXTURE_2D, sunTexId);

  if (!GL::textureDataFromFile("@caelum/sun.dds")) {
    OZ_ERROR("Failed to load sun texture");
  }

  glGenTextures(1, &moonTexId);
  glBindTexture(GL_TEXTURE_2D, moonTexId);

  if (!GL::textureDataFromFile("@caelum/moon.dds")) {
    OZ_ERROR("Failed to load moon texture");
  }

  glGenTextures(6, skyboxTexIds);

  for (int i = 0; i < 6; ++i) {
    File file = String::format("%s/%s.dds", path.c(), SKYBOX_FACES[i]);

    glBindTexture(GL_TEXTURE_2D, skyboxTexIds[i]);
    if (!GL::textureDataFromFile(file)) {
      OZ_ERROR("Failed to load skybox texture '%s'", file.c());
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

  axis = Vec3(-Math::sin(orbis.caelum.heading), Math::cos(orbis.caelum.heading), 0.0f);
  originalLightDir = Vec3(-Math::cos(orbis.caelum.heading), -Math::sin(orbis.caelum.heading), 0.0f);

  celestialShaderId = liber.shaderIndex("celestial");

  nightColour = Vec4(NIGHT_COLOUR);
  nightLuminance = (nightColour.x + nightColour.y + nightColour.z) / 3.0f;

  OZ_GL_CHECK_ERROR();

  update();
}

void Caelum::unload()
{
  if (id == -1) {
    return;
  }

  glDeleteTextures(6, skyboxTexIds);

  glDeleteTextures(1, &sunTexId);
  glDeleteTextures(1, &moonTexId);

  sunTexId      = 0;
  moonTexId     = 0;

  lightDir      = Vec3(0.0f, 0.0f, 1.0f);

  diffuseColour = Vec4::ONE;
  ambientColour = Vec4::ONE;
  caelumColour  = Vec4::ONE;

  id = -1;
}

Caelum caelum;

}
