--[[
 *  generic.lua
 *  Generic handlers
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
]]--

function healUser100( l )
  ozObjBindUser()
  ozObjAddLife( 100.0 )
end

function healUser100_disposable( l )
  ozObjQuietDestroy()
  ozObjBindUser()
  ozObjAddLife( 100.0 )
end

function spawnSmallExplosion( l )
  ozOrbisForceAddObj( "smallExplosion", ozObjGetPos() );
end

function spawnBigExplosion( l )
  ozOrbisForceAddObj( "bigExplosion", ozObjGetPos() );
end

function item_spawnSmallExplosion( l )
  if ozDynBindParent() then
    ozObjDestroy()
  end
  ozOrbisForceAddObj( "smallExplosion", ozObjGetPos() )
end

function item_spawnBigExplosion( l )
  if ozDynBindParent() then
    ozObjDestroy()
  end
  ozOrbisForceAddObj( "bigExplosion", ozObjGetPos() )
end
