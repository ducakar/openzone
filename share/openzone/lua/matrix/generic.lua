--[[
 *  generic.lua
 *  Generic handlers
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
]]--

function healUser100( l )
  ozObjBindUser()

  if ozBotGetState( OZ_BOT_MECHANICAL_BIT ) then
    ozUseFailed()
  else
    ozObjAddLife( 100.0 )
  end
end

function healUser100_disposable( l )
  ozObjBindUser()

  if ozBotGetState( OZ_BOT_MECHANICAL_BIT ) then
    ozUseFailed()
  else
    ozObjAddLife( 100.0 )

    ozObjBindSelf()
    ozObjQuietDestroy()
  end
end

function spawnSmallExplosion( l )
  ozOrbisAddObj( "smallExplosion", ozObjGetPos() );
end

function spawnBigExplosion( l )
  ozOrbisAddObj( "bigExplosion", ozObjGetPos() );
end
