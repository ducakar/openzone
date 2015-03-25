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
 * @file client/Shape.cc
 */

#include <client/Shape.hh>

#include <client/Shader.hh>

#ifdef OZ_DYNAMICS
# include <ozDynamics/ozDynamics.hh>
#endif

namespace oz
{
namespace client
{

const Shape::Vertex Shape::VERTICES[] = {
  // filled rectangle (base index 0)
  { { 0.0f, 0.0f, 0.0f }, { 0, 1 } },
  { { 1.0f, 0.0f, 0.0f }, { 1, 1 } },
  { { 0.0f, 1.0f, 0.0f }, { 0, 0 } },
  { { 1.0f, 1.0f, 0.0f }, { 1, 0 } },

  // line loop rectangle (base index 4)
  { { 0.0f, 0.0f, 0.0f }, {} },
  { { 1.0f, 0.0f, 0.0f }, {} },
  { { 1.0f, 1.0f, 0.0f }, {} },
  { { 0.0f, 1.0f, 0.0f }, {} },

  // tag box (base index 8)
  { { -1.5f, -1.5f, 0.0f }, {} },
  { { -1.5f, +3.5f, 0.0f }, {} },
  { { -0.5f, -1.5f, 0.0f }, {} },
  { { +3.5f, -1.5f, 0.0f }, {} },

  { { +1.5f, -1.5f, 0.0f }, {} },
  { { -3.5f, -1.5f, 0.0f }, {} },
  { { +1.5f, -0.5f, 0.0f }, {} },
  { { +1.5f, +3.5f, 0.0f }, {} },

  { { +1.5f, +1.5f, 0.0f }, {} },
  { { -3.5f, +1.5f, 0.0f }, {} },
  { { +1.5f, +0.5f, 0.0f }, {} },
  { { +1.5f, -3.5f, 0.0f }, {} },

  { { -1.5f, +1.5f, 0.0f }, {} },
  { { +3.5f, +1.5f, 0.0f }, {} },
  { { -1.5f, +0.5f, 0.0f }, {} },
  { { -1.5f, -3.5f, 0.0f }, {} },

  // sprite (base index 24)
  { { -1.0f, -1.0f, 0.0f }, { 0, 1 } },
  { { +1.0f, -1.0f, 0.0f }, { 1, 1 } },
  { { -1.0f, +1.0f, 0.0f }, { 0, 0 } },
  { { +1.0f, +1.0f, 0.0f }, { 1, 0 } },

  // box (base index 28)
  { { -1.0f, -1.0f, -1.0f }, {} },
  { { -1.0f, -1.0f, +1.0f }, {} },
  { { -1.0f, +1.0f, -1.0f }, {} },
  { { -1.0f, +1.0f, +1.0f }, {} },
  { { +1.0f, -1.0f, -1.0f }, {} },
  { { +1.0f, -1.0f, +1.0f }, {} },
  { { +1.0f, +1.0f, -1.0f }, {} },
  { { +1.0f, +1.0f, +1.0f }, {} },

  // skybox (base index 36)
  { { +1.0f, +1.0f, -1.0f }, { 0, 1 } },
  { { +1.0f, -1.0f, -1.0f }, { 1, 1 } },
  { { +1.0f, -1.0f, +1.0f }, { 1, 0 } },
  { { +1.0f, +1.0f, +1.0f }, { 0, 0 } },

  { { -1.0f, -1.0f, -1.0f }, { 0, 1 } },
  { { -1.0f, +1.0f, -1.0f }, { 1, 1 } },
  { { -1.0f, +1.0f, +1.0f }, { 1, 0 } },
  { { -1.0f, -1.0f, +1.0f }, { 0, 0 } },

  { { -1.0f, +1.0f, -1.0f }, { 0, 1 } },
  { { +1.0f, +1.0f, -1.0f }, { 1, 1 } },
  { { +1.0f, +1.0f, +1.0f }, { 1, 0 } },
  { { -1.0f, +1.0f, +1.0f }, { 0, 0 } },

  { { +1.0f, -1.0f, -1.0f }, { 0, 1 } },
  { { -1.0f, -1.0f, -1.0f }, { 1, 1 } },
  { { -1.0f, -1.0f, +1.0f }, { 1, 0 } },
  { { +1.0f, -1.0f, +1.0f }, { 0, 0 } },

  { { -1.0f, +1.0f, +1.0f }, { 0, 1 } },
  { { +1.0f, +1.0f, +1.0f }, { 1, 1 } },
  { { +1.0f, -1.0f, +1.0f }, { 1, 0 } },
  { { -1.0f, -1.0f, +1.0f }, { 0, 0 } },

  { { -1.0f, -1.0f, -1.0f }, { 0, 1 } },
  { { +1.0f, -1.0f, -1.0f }, { 1, 1 } },
  { { +1.0f, +1.0f, -1.0f }, { 1, 0 } },
  { { -1.0f, +1.0f, -1.0f }, { 0, 0 } }
};

const ushort Shape::INDICES[] = {
  /*
   * Full box (GL_TRIANGLE_STRIP)
   */

  // left
  28 + 0,
  28 + 1,
  28 + 2,
  28 + 3,
  // back
  28 + 6,
  28 + 7,
  // right
  28 + 4,
  28 + 5,
  // front
  28 + 0,
  28 + 1,
  28 + 1,
  // bottom
  28 + 0,
  28 + 0,
  28 + 2,
  28 + 4,
  28 + 6,
  28 + 6,
  // top
  28 + 1,
  28 + 1,
  28 + 5,
  28 + 3,
  28 + 7,

  /*
   * Wire box (GL_LINES)
   */

  // parallel to z
  28 + 0,
  28 + 1,
  28 + 2,
  28 + 3,
  28 + 4,
  28 + 5,
  28 + 6,
  28 + 7,
  // parallel to y
  28 + 0,
  28 + 2,
  28 + 1,
  28 + 3,
  28 + 4,
  28 + 6,
  28 + 5,
  28 + 7,
  // parallel to x
  28 + 0,
  28 + 4,
  28 + 1,
  28 + 5,
  28 + 2,
  28 + 6,
  28 + 3,
  28 + 7,

  /*
   * Skybox (GL_TRIANGLE_STRIP)
   */

  // +x
  36 + 0,
  36 + 1,
  36 + 3,
  36 + 2,

  // -x
  40 + 0,
  40 + 1,
  40 + 3,
  40 + 2,

  // +y
  44 + 0,
  44 + 1,
  44 + 3,
  44 + 2,

  // -y
  48 + 0,
  48 + 1,
  48 + 3,
  48 + 2,

  // +z
  52 + 0,
  52 + 1,
  52 + 3,
  52 + 2,

  // -z
  56 + 0,
  56 + 1,
  56 + 3,
  56 + 2
};

Shape::Shape() :
  vbo(0), ibo(0)
{}

void Shape::bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glEnableVertexAttribArray(Shader::POSITION);
  glVertexAttribPointer(Shader::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        static_cast<char*>(nullptr) + offsetof(Vertex, pos));

  glEnableVertexAttribArray(Shader::TEXCOORD);
  glVertexAttribPointer(Shader::TEXCOORD, 2, GL_SHORT, GL_FALSE, sizeof(Vertex),
                        static_cast<char*>(nullptr) + offsetof(Vertex, texCoord));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void Shape::unbind() const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Shape::colour(const Vec4& c)
{
  glUniformMatrix4fv(uniform.colour, 1, GL_FALSE, Mat4::scaling(c));
}

void Shape::colour(float r, float g, float b, float a)
{
  glUniformMatrix4fv(uniform.colour, 1, GL_FALSE,
                     Mat4(   r, 0.0f, 0.0f, 0.0f,
                          0.0f,    g, 0.0f, 0.0f,
                          0.0f, 0.0f,    b, 0.0f,
                          0.0f, 0.0f, 0.0f,    a));
}

void Shape::fill(float x, float y, float width, float height)
{
  tf.model = Mat4::translation(Vec3(x, y, 0.0f));
  tf.model.scale(Vec3(width, height, 0.0f));
  tf.apply();

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Shape::fill(int x, int y, int width, int height)
{
  fill(float(x), float(y), float(width), float(height));
}

void Shape::rect(float x, float y, float width, float height)
{
  tf.model = Mat4::translation(Vec3(x + 0.5f, y + 0.5f, 0.0f));
  tf.model.scale(Vec3(width - 1.0f, height - 1.0f, 0.0f));
  tf.apply();

  glDrawArrays(GL_LINE_LOOP, 4, 4);
}

void Shape::rect(int x, int y, int width, int height)
{
  rect(float(x), float(y), float(width), float(height));
}

void Shape::tag(float minX, float minY, float maxX, float maxY)
{
  tf.model = Mat4::translation(Vec3(minX, minY, 0.0f));
  tf.apply();

  glDrawArrays(GL_LINES, 8, 4);

  tf.model = Mat4::translation(Vec3(maxX, minY, 0.0f));
  tf.apply();

  glDrawArrays(GL_LINES, 12, 4);

  tf.model = Mat4::translation(Vec3(maxX, maxY, 0.0f));
  tf.apply();

  glDrawArrays(GL_LINES, 16, 4);

  tf.model = Mat4::translation(Vec3(minX, maxY, 0.0f));
  tf.apply();

  glDrawArrays(GL_LINES, 20, 4);
}

void Shape::quad(float dimX, float dimY)
{
  tf.model.scale(Vec3(dimX, 1.0f, dimY));
  tf.apply();

  glDrawArrays(GL_TRIANGLE_STRIP, 24, 4);
}

void Shape::box(const AABB& bb)
{
  tf.model = Mat4::translation(bb.p - Point::ORIGIN);
  tf.model.scale(bb.dim);
  tf.apply();

  glDrawElements(GL_TRIANGLE_STRIP, 22, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 0);
}

void Shape::wireBox(const AABB& bb)
{
  tf.model = Mat4::translation(bb.p - Point::ORIGIN);
  tf.model.scale(bb.dim);
  tf.apply();

  glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 22);
}

void Shape::skyBox(uint* texIds)
{
  tf.apply();

  glBindTexture(GL_TEXTURE_2D, texIds[0]);
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 46);

  glBindTexture(GL_TEXTURE_2D, texIds[1]);
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 50);

  glBindTexture(GL_TEXTURE_2D, texIds[2]);
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 54);

  glBindTexture(GL_TEXTURE_2D, texIds[3]);
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 58);

  glBindTexture(GL_TEXTURE_2D, texIds[4]);
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 62);

  glBindTexture(GL_TEXTURE_2D, texIds[5]);
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 66);
}

#ifdef OZ_DYNAMICS

void Shape::object(const Point& pos, const Mat3& rot, const void* shape_)
{
  const oz::Shape* shape = static_cast<const oz::Shape*>(shape_);

  if (shape->type == oz::Shape::BOX) {
    const Box* box = static_cast<const Box*>(shape);

    tf.model = Mat4::translation(pos - Point::ORIGIN);
    tf.model = tf.model * Mat4(rot);
    tf.model.scale(box->ext);
    tf.apply();

    colour(0.5f, 0.5f, 0.5f, 1.0f);
    glDrawElements(GL_TRIANGLE_STRIP, 22, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 0);
    colour(1.0f, 0.0f, 0.0f, 1.0f);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 22);
  }
  else if (shape->type == oz::Shape::CAPSULE) {
    const Capsule* capsule = static_cast<const Capsule*>(shape);

    tf.model = Mat4::translation(pos - Point::ORIGIN);
    tf.model = tf.model * Mat4(rot);
    tf.model.scale(Vec3(capsule->radius, capsule->radius, capsule->ext + capsule->radius));
    tf.apply();

    colour(0.5f, 0.5f, 0.5f, 1.0f);
    glDrawElements(GL_TRIANGLE_STRIP, 22, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 0);
    colour(1.0f, 0.0f, 0.0f, 1.0f);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, static_cast<ushort*>(nullptr) + 22);

  }
  else if (shape->type == oz::Shape::COMPOUND) {
    const Compound* compound = static_cast<const Compound*>(shape);

    for (const Compound::Child& child : *compound) {
      object(pos + rot * child.off, rot * child.rot, child.shape);
    }
  }
}

#endif

void Shape::init()
{
  MainCall() << [&]
  {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  };
}

void Shape::destroy()
{
  MainCall() << [&]
  {
    if (vbo != 0) {
      glDeleteBuffers(1, &ibo);
      glDeleteBuffers(1, &vbo);

      ibo = 0;
      vbo = 0;
    }
  };
}

Shape shape;

}
}
