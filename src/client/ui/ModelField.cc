/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/ui/ModelField.cc
 */

#include <client/ui/ModelField.hh>

#include <common/Timer.hh>
#include <client/Input.hh>
#include <client/Shape.hh>
#include <client/SMM.hh>
#include <client/Context.hh>
#include <client/ui/Style.hh>

namespace oz
{
namespace client
{
namespace ui
{

const float ModelField::DEFAULT_ROTATION   = -Math::TAU / 8.0f;
const float ModelField::ROTATION_VEL       = 1.30f * Timer::TICK_TIME;
const float ModelField::ROTATION_SMOOTHING = 0.75f;

void ModelField::onVisibilityChange( bool )
{
  currRot       = defaultRot;
  nextRot       = defaultRot;
  isHighlighted = false;
  isClicked     = false;
}

bool ModelField::onMouseEvent()
{
  if( !input.keys[Input::KEY_UI_ALT] ) {
    nextRot       = angleWrap( nextRot + ROTATION_VEL );
    isHighlighted = true;

    if( input.leftClick && callback != nullptr ) {
      isClicked = true;

      callback( this );
    }
  }
  return true;
}

void ModelField::onDraw()
{
  if( isClicked ) {
    shape.colour( style.colours.buttonClicked );
  }
  else if( isHighlighted ) {
    shape.colour( style.colours.buttonHover );
  }
  else {
    shape.colour( style.colours.button );
  }

  shape.fill( x, y, width, height );

  if( bsp != nullptr || model >= 0 ) {
    BSP*  bspModel = nullptr;
    SMM*  smmModel = nullptr;
    float dim;

    if( bsp != nullptr ) {
      bspModel = context.requestBSP( bsp );
      dim      = bspModel->dim().fastN();
    }
    else {
      smmModel = context.requestSMM( model );
      dim      = smmModel->dim().fastN();
    }

    float scale = float( width / 2 ) / dim;

    currRot = nextRot + ROTATION_SMOOTHING * angleDiff( currRot, nextRot );

    tf.model = Mat44::translation( Vec3( float( x + width / 2 ), float( y + height / 2 ), 0.0f ) );
    tf.model.scale( Vec3( scale, scale, scale ) );
    tf.model.rotateX( -Math::TAU / 8.0f );
    tf.model.rotateZ( currRot );

    if( bsp != nullptr ) {
      if( bspModel->isLoaded() ) {
        bspModel->schedule( nullptr );
      }
    }
    else {
      if( smmModel->isLoaded() ) {
        smmModel->schedule( -1 );
      }
    }

    shape.unbind();

    glEnable( GL_DEPTH_TEST );

    Mesh::drawScheduled( Mesh::SOLID_BIT | Mesh::ALPHA_BIT );
    Mesh::clearScheduled();

    glDisable( GL_DEPTH_TEST );

    shape.bind();
    shader.program( shader.plain );
  }

  nextRot       = isHighlighted ? nextRot : defaultRot;
  isHighlighted = false;
  isClicked     = false;
}

ModelField::ModelField( Callback* callback_, int width, int height ) :
  Area( width, height ), callback( callback_ ), bsp( nullptr ),
  defaultRot( DEFAULT_ROTATION ), currRot( DEFAULT_ROTATION ), nextRot( DEFAULT_ROTATION ),
  isHighlighted( false ), isClicked( false )
{}

void ModelField::setDefaultRotation( float defaultRotation )
{
  defaultRot = defaultRotation;

  if( !isHighlighted ) {
    currRot = defaultRot;
    nextRot = defaultRot;
  }
}

void ModelField::setCallback( Callback* callback_ )
{
  callback = callback_;
}

void ModelField::setBSP( const oz::BSP* bsp_ )
{
  if( bsp_ != bsp ) {
    bsp     = bsp_;
    model   = -1;

    currRot = defaultRot;
    nextRot = defaultRot;
  }
}

void ModelField::setModel( int model_ )
{
  if( model_ != model ) {
    bsp     = nullptr;
    model   = model_;

    currRot = defaultRot;
    nextRot = defaultRot;
  }
}

}
}
}
