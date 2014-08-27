--
-- OpenZone - simple cross-platform FPS/RTS game engine.
--
-- Copyright © 2002-2014 Davorin Učakar
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.
--

-- lua/matrix/oz_main.lua
--
-- Handlers for oz_main objects.

--
-- Helper functions
--

function meleeAttack(aabbFwdTransl, aabbDim, minDamage, maxDamage)
  ozBindUser()

  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozBotGetDir()

  ozOrbisBindOverlaps(OZ_OBJECTS_BIT,
                      pX + aabbFwdTransl * vX, pY + aabbFwdTransl * vY, pZ + aabbFwdTransl * vZ,
                      aabbDim, aabbDim, aabbDim)

  while ozBindNextObj() do
    if not (ozObjIsSelf() or ozObjIsUser()) then
      ozObjDamage(minDamage + math.random() * (maxDamage - minDamage))
    end
  end

  return true
end

function gunAttack(bulletFragPool, velocity, stillDisp, movingDisp)
  ozBindUser()
  if ozBotHasState(OZ_BOT_SUBMERGED_BIT) then
    return false
  end

  local disp       = ozBotHasState(OZ_BOT_MOVING_BIT) and movingDisp or stillDisp
  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = Vec3.random(disp)

  ozOrbisAddFrag(OZ_FORCE, bulletFragPool,
                 pX, pY, pZ,
                 vX * velocity + dX, vY * velocity + dY, vZ * velocity + dZ)

  return true
end

function vehicleGunAttack(bulletFragPool, velocity, disp)
  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozVehicleGetDir()
  local dX, dY, dZ = Vec3.random(disp)

  ozOrbisAddFrag(OZ_FORCE, bulletFragPool,
                 pX, pY, pZ,
                 vX * velocity + dX, vY * velocity + dY, vZ * velocity + dZ)
end

--
-- Miscellaneous
--

function firstAid_onUse(l)
  ozBindUser()

  if ozBotHasState(OZ_BOT_MECHANICAL_BIT) then
    return false
  else
    ozObjAddLife(100.0)

    ozBindSelf()
    ozObjDestroy(true)
  end

  return true
end

function cvicek_onUse(l)
  ozBindUser()

  if ozBotHasState(OZ_BOT_MECHANICAL_BIT) then
    return false
  else
    ozObjAddLife(50)
    ozBotAddStamina(30)

    ozBindSelf()
    ozObjDestroy(true)
  end

  return true
end

function bomb_onUse(l)
  if not l.ticks then
    l.ticks = 250 -- 5 s
    ozObjEnableUpdate(true)
  else
    l.ticks = nil
    ozObjEnableUpdate(false)
  end

  return true
end

function bomb_onUpdate(l)
  if l.ticks ~= 0 then
    l.ticks = l.ticks - 1
  else
    ozObjDestroy()
  end
end

function serviceStation_onUse(l)
  local pX, pY, pZ = ozObjGetPos()

  ozBindUser()
  ozBotRearm()

  if ozBotHasState(OZ_BOT_MECHANICAL_BIT) then
    ozBotHeal()
  end

  ozOrbisBindOverlaps(OZ_OBJECTS_BIT, pX, pY, pZ, 5, 5, 2)
  while ozBindNextObj() do
    if ozObjHasFlag(OZ_VEHICLE_BIT) then
      ozVehicleService()
    end
  end

  return true
end

--
-- Explosions
--

function spawnSmallExplosion(l)
  ozObjAddEvent(OZ_EVENT_SHAKE, 0.5)
  ozOrbisAddObj(OZ_FORCE, "smallExplosion", ozObjGetPos())
end

function spawnBigExplosion(l)
  ozObjAddEvent(OZ_EVENT_FLASH, 0.5)
  ozObjAddEvent(OZ_EVENT_SHAKE, 1.0)
  ozOrbisAddObj(OZ_FORCE, "bigExplosion", ozObjGetPos())
end

function smallExplosion_onUpdate(l)
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozObjDestroy(true)
    end
  else
    l.ticks = 25

    ozObjAddEvent(OZ_EVENT_CREATE, 1)
    ozObjBindOverlaps(OZ_OBJECTS_BIT, 8)

    while ozBindNextObj() do
      if not ozObjIsSelf() then
        local distance = ozObjDistFromSelf()

        if distance < 8 then
          distance = 8 - distance

          if ozObjIsVisibleFromSelf() then
            ozObjDamage(100 + 12*distance)

            if distance < 7.9 and ozObjHasFlag(OZ_DYNAMIC_BIT) then
              local dirX, dirY, dirZ = ozObjDirFromSelf()
              distance = 2 * distance

              ozDynAddMomentum(dirX * distance, dirY * distance, dirZ * distance)
            end
          end
        end
      end
    end
  end
end

function bigExplosion_onUpdate(l)
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozObjDestroy(true)
    end
  else
    l.ticks = 25

    ozObjAddEvent(OZ_EVENT_CREATE, 1)
    ozObjBindOverlaps(OZ_STRUCTS_BIT + OZ_OBJECTS_BIT, 20)

    while ozBindNextStr() do
      ozStrDamage(1000)
    end

    while ozBindNextObj() do
      if not ozObjIsSelf() then
        local distance = ozObjDistFromSelf()

        if distance < 20 then
          distance = 20 - distance

          ozObjDamage(100 + 2*distance)

          if ozObjIsVisibleFromSelf() then
            ozObjDamage(100 + 6*distance)

            if distance < 19.9 and ozObjHasFlag(OZ_DYNAMIC_BIT) then
              local dirX, dirY, dirZ = ozObjDirFromSelf()

              ozDynAddMomentum(dirX * distance, dirY * distance, dirZ * distance)
            end
          end
        end
      end
    end
  end
end

--
-- Weapons
--

function melee_onShot(l)
  return meleeAttack(0.4, 0.4, 100, 125)
end

function plasmagun_onUpdate(l)
  if not l.tick then
    l.tick = 80
  elseif l.tick ~= 0 then
    l.tick = l.tick - 1
  else
    l.tick = 80

    ozWeaponAddRounds(1)

    if ozWeaponGetRounds() == ozWeaponMaxRounds() then
      ozObjEnableUpdate(false)
    end
  end
end

function plasmagun_onShot(l)
  ozObjEnableUpdate(true)

  return gunAttack("plasmaBullet", 230, 2, 20)
end

function blaster_onShot(l)
  return gunAttack("blasterBullet", 220, 2, 20)
end

function hyperblaster_onShot(l)
  return gunAttack("hyperblasterBullet", 300, 1, 10)
end

function chaingun_onShot(l)
  return gunAttack("chaingunBullet", 250, 2, 20)
end

function grenadeLauncher_onShot(l)
  ozBindUser()
  if ozBotHasState(OZ_BOT_SUBMERGED_BIT) then
    return false
  end

  local disp       = ozBotHasState(OZ_BOT_MOVING_BIT) and 5 or 0.5
  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = Vec3.random(disp)

  ozOrbisAddObj(OZ_FORCE, "grenade", pX, pY, pZ)
  ozDynSetMomentum(vX * 30 + dX, vY * 30 + dY, vZ * 30 + dZ)

  return true
end

function axe_onShot(l)
  return meleeAttack(0.5, 0.3, 100, 150)
end

function mace_onShot(l)
  return meleeAttack(0.6, 0.4, 100, 200)
end

function skull_onShot(l)
  return meleeAttack(0.6, 0.4, 100, 300)
end

--
-- Vehicle weapons
--

function vehicle_heavyBlaster_onShot(l)
  vehicleGunAttack("heavyBlasterBullet", 300, 0)
end

function vehicle_gatling_onShot(l)
  vehicleGunAttack("gatlingBullet", 200, 5)
end

function vehicle_cannon_onShot(l)
  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozVehicleGetDir()

  ozOrbisAddObj(OZ_FORCE, "shell", pX, pY, pZ)
  ozDynSetMomentum(vX * 150, vY * 150, vZ * 150)
end

function shell_onUpdate(l)
  if not l.ticks then
    l.ticks = 300
  elseif l.ticks > 0 then
    l.ticks = l.ticks - 1
  else
    ozObjDestroy()
  end
end

--
-- Name generators
--

Names = {
  random = function(list)
    return list[ math.random(#list) ]
  end,

  troll = {
    first = {
      "Äg", "Agh", "Bog", "Borb", "Brag", "Brok", "Dak", "Drog", "Frok", "Ga", "Gark", "Gnarf",
      "Grar", "Grokk", "Grumph", "Gulk", "Hak", "Hask", "Hoth", "Hug", "Hu", "Kak", "Kar", "Krak",
      "Krug", "Kub", "Lok", "Luk", "Nak", "Nuk", "Pag", "Reck", "Rok", "Ruk", "Sark", "Shak",
      "Shuf", "Targ", "Thog", "Thruf", "Thur", "Tohg", "Torg", "Trok", "Tsok", "Tuk", "Ugg", "Urg",
      "Urgh", "Urk", "Vak", "Zog", "Zuug"
    },
    sex = {
      "'koh", "'keh"
    },
    last = {
      "Äh", "Bah", "Goh", "Gah", "Kah", "Kuh", "Ruh", "Stuh", "Tuh", "Üh"
    }
  },

  lizard = {
    first = {
      "Amprixta", "Anexir", "Anitraz", "Axiz", "Bzz'Kza", "Chamil", "Cleezi", "Clezz", "Fazzis",
      "Flixta", "Flizzil", "Frikes", "Frizzle", "Hasz", "Heffez", "Hertrazzir", "Hesz", "Hezzir",
      "Hezzis", "Inexis", "Jizz", "Kaliez", "Kepzs", "Kersezz", "Kertrasz", "Kezz", "Klexaz",
      "Krisess", "Lazki", "Lixeez", "Mexiss", "Moxanzz", "Naxisz", "Pekzs", "Plaxis", "Presch",
      "Sailik", "Salik", "Saprazz", "Satras", "Skeely", "Sklizle", "Skrez", "Sprizz", "Ssexur",
      "Ssizer", "Sszasz", "Sterizz", "Talerez", "Tezzaz", "Tirasch", "Trezz", "Venezz", "Vriss",
      "Waks", "Xaffrasz", "Xartrez", "Xasz", "Xirasz", "Xirr", "Xirtras", "Xirtrez", "Xirz",
      "Zandler", "Zizzasz", "Zslap", "Zzalkz", "Zzupde"
    },
    last = {
      "Arix", "Fizztrax", "Hix", "Irix", "Jezzix", "Kernix", "Kerx", "Kerxenix", "Klezyx", "Krarax",
      "Krenarex", "Krex", "Krinex", "Laizix", "Merax", "Nix", "Plesix", "Salanix", "Sandix",
      "Skalix", "Skandix", "Skazix", "Skeezix", "Slizilx", "Ssorix", "Tarex", "Tarnix", "Tirax",
      "Tirix", "Xaztex", "Xerxix", "Zedrix", "Zilrix",
    }
  },

  human = {
    "Addræcyn", "Addrænvan", "Addrær", "Addrærcyn", "Addraryn", "Addreddry", "Addredry",
    "Addregwyn", "Addrenyc", "Addreoddry", "Addreoddyn", "Addreonyc", "Addreorcyn", "Addreran",
    "Addribryn", "Addriddyn", "Addrocyn", "Addroryn", "Addrunvan", "Addrurcyn", "Addryllyn",
    "Addrynvan", "Æthacyn", "Æthadry", "Æthæc", "Æthæran", "Æthæryn", "Æthagwyn", "Æthanry",
    "Ætharcyn", "Æthec", "Æthellyn", "Æthenvan", "Ætheoc", "Ætheollyn", "Ætheonyc", "Ætheorcyn",
    "Æthercyn", "Ætherrænt", "Æthibryn", "Æthiddry", "Æthircyn", "Æthobryn", "Æthoddyn", "Æthonnyn",
    "Æthuc", "Æthudry", "Æthugwyn", "Æthun", "Æthunry", "Æthydry", "Æthynyc", "Blac", "Bladoc",
    "Blæc", "Blædry", "Blanry", "Blebryn", "Bledoc", "Blemyr", "Blennyn", "Blenvan", "Bleollyn",
    "Blercyn", "Blidd", "Bliddry", "Blillyn", "Blinvan", "Blollyn", "Blubryn", "Blucyn", "Bludry",
    "Blullyn", "Bluran", "Blybryn", "Blydd", "Blygwyn", "Blymyr", "Blyr", "Bucyn", "Cac", "Cadry",
    "Cæbryn", "Cædry", "Cæran", "Cærcyn", "Car", "Carac", "Caraddry", "Caradoc", "Carædry",
    "Carænnyn", "Cararyn", "Caredd", "Careddry", "Caregwyn", "Caren", "Careobryn", "Careogwyn",
    "Careonvan", "Careorrænt", "Careoryn", "Carercyn", "Caric", "Cariddry", "Carocyn", "Caroddyn",
    "Caror", "Caroran", "Carrænt", "Carudoc", "Carullyn", "Carygwyn", "Caryn", "Cebryn", "Cemyr",
    "Cennyn", "Ceoc", "Ceoddry", "Ceoddyn", "Ceomyr", "Ceonnyn", "Ceonry", "Ceoryn", "Cicyn", "Cin",
    "Cinry", "Coc", "Convan", "Corcyn", "Cubryn", "Cunry", "Curyn", "Cynyc", "Cyryn", "Dac", "Dadd",
    "Dadoc", "Dæddry", "Dædoc", "Dællyn", "Demyr", "Denvan", "Deodd", "Deollyn", "Deonyc",
    "Derrænt", "Dibryn", "Dinnyn", "Dircyn", "Dycyn", "Dyddyn", "Gaddry", "Gæbryn", "Gædry",
    "Gærcyn", "Gagwyn", "Gan", "Gannyn", "Gar", "Gecyn", "Geddyn", "Gegwyn", "Geodry", "Ginvan",
    "Glacyn", "Gladoc", "Glærcyn", "Glarrænt", "Gleddry", "Gleoddyn", "Gleran", "Gliddyn",
    "Glillyn", "Glinry", "Glircyn", "Gloddry", "Gloddyn", "Glonry", "Glonvan", "Glumyr", "Glun",
    "Glunry", "Glunvan", "Glyc", "Glydd", "Glydoc", "Glynry", "Glynvan", "Glyran", "Goc", "Gor",
    "Gubryn", "Gudd", "Gullyn", "Gumyr", "Gur", "Gwadoc", "Gwæc", "Gwæddyn", "Gwan", "Gweddyn",
    "Gwegwyn", "Gwellyn", "Gwennyn", "Gwenyc", "Gweocyn", "Gweodd", "Gweodoc", "Gweodry",
    "Gweogwyn", "Gweoran", "Gwidoc", "Gwilam", "Gwodd", "Gwoddyn", "Gwollyn", "Gwor", "Gwucyn",
    "Gwudoc", "Gwumyr", "Gwuran", "Gwybryn", "Gwycyn", "Gwyddry", "Gwydoc", "Gwymyr", "Gwynnyn",
    "Gydoc", "Gyllyn", "Gymyr", "Haldar", "Labryn", "Ladoc", "Lællyn", "Lan", "Lannyn", "Laran",
    "Lec", "Lemyr", "Lenvan", "Leogwyn", "Lercyn", "Ligwyn", "Lin", "Liryn", "Lonnyn", "Lorrænt",
    "Luddry", "Ludoc", "Lunnyn", "Lunvan", "Lurrænt", "Mac", "Maddyn", "Mænnyn", "Manry", "Manyc",
    "Marcyn", "Mec", "Menvan", "Meollyn", "Meon", "Meonnyn", "Meorrænt", "Middry", "Midry", "Mimyr",
    "Modd", "Moddry", "Monry", "Moran", "Morcyn", "Mubryn", "Mudoc", "Mugwyn", "Murcyn", "Mydoc",
    "Mygwyn", "Myn", "Myrrænt", "Owac", "Owadd", "Owaddyn", "Owæcyn", "Owædry", "Owain", "Owarcyn",
    "Owaryn", "Owecyn", "Owedry", "Oweomyr", "Oweor", "Oweorcyn", "Oweran", "Owercyn", "Owidry",
    "Owinvan", "Owinyc", "Owodd", "Owoddry", "Owogwyn", "Owollyn", "Oworan", "Oworcyn", "Oworrænt",
    "Owuddry", "Owuddyn", "Owugwyn", "Owur", "Owyran", "Rabryn", "Radd", "Ranvan", "Rar", "Reoddyn",
    "Reodry", "Rhæcyn", "Rhædoc", "Rhæmyr", "Rhærrænt", "Rhanry", "Rharcyn", "Rhenry", "Rhenvan",
    "Rhenyc", "Rheodd", "Rheoddyn", "Rheollyn", "Rheor", "Rheoran", "Rheorrænt", "Rheran",
    "Rherrænt", "Rhobryn", "Rhodry", "Rhollyn", "Rhonvan", "Rhubryn", "Rhugwyn", "Rhunyc", "Rhur",
    "Rhygwyn", "Rhyllyn", "Rhynyc", "Rhyrcyn", "Rhyrrænt", "Rocyn", "Roddyn", "Romyr", "Ron",
    "Ronry", "Rubryn", "Ruddry", "Rumyr", "Run", "Rurcyn", "Rybryn", "Rycyn", "Ryddry", "Rygwyn",
    "Rynnyn", "Rynry", "Sæc", "Sællyn", "Sæmyr", "Sænvan", "Særcyn", "Sanyc", "Saran", "Sarrænt",
    "Secyn", "Seddyn", "Sedry", "Sellyn", "Sennyn", "Seoddry", "Seorcyn", "Sercyn", "Siddry",
    "Simyr", "Siryn", "Sodd", "Sodry", "Soran", "Suc", "Sudd", "Surcyn", "Sydd", "Syran", "Syryn",
    "Tabryn", "Tæc", "Tædd", "Tædoc", "Tæmyr", "Tænvan", "Tærcyn", "Tanry", "Tarcyn", "Teddyn",
    "Tegwyn", "Ten", "Tennyn", "Tenvan", "Teobryn", "Teoddyn", "Teor", "Teorcyn", "Terrænt",
    "Tinry", "Tinvan", "Tiryn", "Todd", "Tudd", "Tuddry", "Tudoc", "Tunvan", "Turrænt", "Tyddyn",
    "Vaddyn", "Væddyn", "Vædry", "Vænnyn", "Varcyn", "Ven", "Vennyn", "Veocyn", "Veoddyn", "Veodry",
    "Veogwyn", "Veomyr", "Vinvan", "Vinyc", "Virrænt", "Vobryn", "Vogwyn", "Vonry", "Vuddyn",
    "Vugwyn", "Vyc", "Vygwyn", "Vyrcyn", "Yracyn", "Yræc", "Yran", "Yrannyn", "Yranvan", "Yraryn",
    "Yredd", "Yreddyn", "Yregwyn", "Yreryn", "Yrinvan", "Yrirrænt", "Yroddry", "Yrullyn", "Yrumyr",
    "Yrunnyn", "Yrunvan", "Yryllyn", "Yrymyr", "Yrynyc", "Yryrcyn"
  },

  ogre = {
    "Akoark", "Akort", "Akzalk", "Arkarm", "Barkuk", "Blokkar", "Borkuk", "Bukkak", "Bulruk",
    "Corkkar", "Delkkak", "Garkuk", "Gnukk", "Goruk", "Grak", "Gurk", "Gurm", "Kalknix", "Karak",
    "Karbuk", "Kargnak", "Karterak", "Kayrak", "Kelkrar", "Kerta", "Kilkrar", "Kingrok", "Kirk",
    "Klud", "Kokkan", "Kolk", "Komak", "Korgnak", "Kork", "Koruck", "Kramak", "Krog", "Krukrak",
    "Krumuk", "Kuknuk", "Kurkur", "Kurmak", "Makron", "Markaak", "Markuk", "Merknik", "Nargak",
    "Olk", "Orkut", "Reknak", "Takolak", "Trabuk", "Trakkon", "Urkar", "Urkark"
  }
}

function hobgoblinName()
  return Names.random(Names.troll.first) .. Names.random(Names.troll.sex) ..
         " " .. Names.random(Names.troll.last)
end

function lizardName()
  return Names.random(Names.lizard.first) .. " " .. Names.random(Names.lizard.last)
end

function humanName()
  return Names.random(Names.human)
end

function ogreName()
  return Names.random(Names.ogre)
end

function droidName()
  return string.char(math.random(65, 90), math.random(65, 90)) .. "-" ..
         math.random(0, 9) .. math.random(0, 9) .. math.random(0, 9)
end

function cyborgName()
  return Names.random(Names.troll.first) .. "-" ..
         string.char(math.random(86, 90)) .. math.random(0, 9) .. math.random(0, 9)
end
