/*
 *  particles.frag
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec4 exColour;

void main()
{
  gl_FragData[0] = exColour;
}
