/*
 *  stars.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying float azimuth;

void main()
{
//   if( azimuth < 0.0 ) {
//     discard;
//   }

  gl_FragColor = oz_DiffuseMaterial;
}
