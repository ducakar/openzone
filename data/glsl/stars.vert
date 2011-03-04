/*
 *  stars.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying float azimuth;

void main()
{
  azimuth = gl_Vertex.z;
  gl_Position = ftransform();
}
