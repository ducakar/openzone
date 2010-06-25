/*
 *  SparkModel.cpp
 *
 *  Visual particle generator
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.hpp"

#include "client/SparkModel.hpp"

#include "client/Frustum.hpp"

namespace oz
{
namespace client
{

//  void SparkGenRender::createSpark( int i )
//  {
//    float velocitySpread2 = sparkGen->velocitySpread * 0.5f;
//    Vec3 velDisturb = Vec3( sparkGen->velocitySpread * Math::frand() - velocitySpread2,
//                            sparkGen->velocitySpread * Math::frand() - velocitySpread2,
//                            sparkGen->velocitySpread * Math::frand() - velocitySpread2 );
//
//    Vec3 colorDisturb = Vec3( sparkGen->colorSpread.x * Math::frand(),
//                              sparkGen->colorSpread.y * Math::frand(),
//                              sparkGen->colorSpread.z * Math::frand() );
//
//    sparks[i].p = sparkGen->p;
//    sparks[i].velocity = sparkGen->velocity + velDisturb;
//    sparks[i].color = sparkGen->color + colorDisturb;
//    sparks[i].lifeTime = sparkGen->lifeTime;
//  }
//
//  SparkGenRender::SparkGenRender( oz::SparkGen* sparkGen_ ) : sparkGen( sparkGen_ )
//  {
//    sparks = new Spark[sparkGen->number];
//    startMillis = float( timer.millis );
//    nSparks = 0;
//
//    for( int i = 0; i < sparkGen->number; ++i ) {
//      sparks[i].lifeTime = 0.0f;
//    }
//  }
//
//  SparkGenRender::SparkGenRender( const SparkGenRender& sparkGenRender ) :
//      sparkGen( sparkGenRender.sparkGen ),
//      startMillis( sparkGenRender.startMillis ),
//      sparksPerTick( sparkGenRender.sparksPerTick ),
//      nSparks( sparkGenRender.nSparks )
//  {
//    sparks = new Spark[nSparks];
//    aCopy( sparks, sparkGenRender.sparks, nSparks );
//  }
//
//  SparkGenRender::~SparkGenRender()
//  {
//    delete[] sparks;
//  }
//
//  void SparkGenRender::draw()
//  {
//    float sparkDim = sparkGen->sparkDim;
//
//    glPushMatrix();
//
//    glMultMatrixf( camera.rotMat );
//
//    for( int i = 0; i < nSparks; ++i ) {
//      Vec3& p = sparks[i].p;
//
//      glTranslatef( p.x, p.y, p.z );
//
//      glBegin( GL_QUADS );
//        glTexCoord2f( 0.0f, 0.0f );
//        glVertex3f( -sparkDim, -sparkDim, 0.0f );
//        glTexCoord2f( 0.0f, 1.0f );
//        glVertex3f( +sparkDim, -sparkDim, 0.0f );
//        glTexCoord2f( 1.0f, 1.0f );
//        glVertex3f( +sparkDim, +sparkDim, 0.0f );
//        glTexCoord2f( 1.0f, 0.0f );
//        glVertex3f( -sparkDim, +sparkDim, 0.0f );
//      glEnd();
//
//      glTranslatef( -p.x, -p.y, -p.z );
//    }
//
//    glPopMatrix();
//  }
//
//  void SparkGenRender::update() {
//    if( nSparks != sparkGen->number ) {
//      int desiredNSparks = int( ( timer.millis - startMillis ) * sparksPerTick );
//
//      nSparks = min( desiredNSparks, sparkGen->number );
//    }
//    for( int i = 0; i < nSparks; ++i ) {
//      sparks[i].lifeTime -= Timer::FRAME_TIME;
//
//      if( sparks[i].lifeTime <= 0.0f ) {
//        createSpark( i );
//      }
//    }
//  }

}
}
