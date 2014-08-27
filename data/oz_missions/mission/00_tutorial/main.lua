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

-- mission/00_tutorial/main.lua
--
-- Tutorial mission.

function onUpdate()
end

function onCreate()
  ozTerraLoad("sample")

  ozCaelumLoad("earth")
  ozCaelumSetHeading(math.pi / 8)
  ozCaelumSetPeriod(3600)
  ozCaelumSetTime(450)

  local me = ozOrbisAddObj(OZ_FORCE, "beast", 78, -90, 66.40, OZ_NORTH, true)
  ozBotSetName(ozProfileGetName())
  ozBotSetMind("")

  local oX, oY, oZ = 80, -80, 69.7
  ozOrbisAddStr(OZ_FORCE, "tutorial_movement", oX, oY, oZ, OZ_SOUTH)

  local infoMove = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 2, oY - 9, oZ - 3)
  local infoClimb = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 2.5, oY + 1, oZ - 2.5)
  local infoLadder = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 4, oY + 3, oZ + 2)
  local infoSwim = ozOrbisAddObj(OZ_FORCE, "datacube", oX + 4, oY + 5, oZ + 2)
  local infoStamina = ozOrbisAddObj(OZ_FORCE, "datacube", oX + 2, oY - 7, oZ + 2)

  ozNirvanaAddMemo(infoMove, ozGettext(
    "1. MOVEMENT\n\n" ..
    "Use W, A, S, D to move and mouse to look around.\n\n" ..
    "You can jump with SPACE, toggle crouch with LCTRL or C and toggle running with LSHIFT.\n\n" ..
    "If you crouch you are a bit lower, so you can go under some obstacles, but you cannot run."))

  ozNirvanaAddMemo(infoClimb, ozGettext(
    "2. LEDGE CLIMBING\n\n" ..
    "You can climb on a ledge if you hold SPACEBAR and W simultaneously."))

  ozNirvanaAddMemo(infoLadder, ozGettext(
    "3. LADDERS\n\n" ..
    "To move up or down a ladder just look in the desired direction while you are touching the " ..
    "ladder.\n\n" ..
    "If you move away from the ladder, you will fall from it.\n\n" ..
    "Be careful when you want to go down the ladder. Approach the ladder slowly or you may miss " ..
    "it and fall.\n\n" ..
    "Note that you cannot climb a ladder when you are carrying some object in your hands."))

  ozNirvanaAddMemo(infoSwim, ozGettext(
    "4. SWIMMING\n\n" ..
    "For swimming you use the same controls as for basic movement.\n\n" ..
    "To dive, just look down and move forwards. You must only watch that you are running (and " ..
    "not crouching), as you must swim fast enough to dive."))

  ozNirvanaAddMemo(infoStamina, ozGettext(
    "5. STAMINA\n\n" ..
    "You probably noticed the blue bar in bottom left corner starts to drain if you are " ..
    "running, jumping, climbing, throwing objects or swimming under water. That is you stamina. " ..
    "If it runs out, you are not able to perform those actions any more.\n\n" ..
    "Be careful it doesn't run out while you are under water or you will start to drawn.\n\n" ..
    "Your stamina is restoring while you are still or walking."))

  local oX, oY, oZ = 60, -92, 69.7
  ozOrbisAddStr(OZ_FORCE, "tutorial_carrying", oX, oY, oZ, OZ_EAST)

  ozOrbisAddObj(OZ_FORCE, "metalCrate", oX + 3.2, oY - 4.5, oZ - 3)
  ozOrbisAddObj(OZ_FORCE, "bigCrate", oX + 1, oY - 3, oZ - 3)
  ozOrbisAddObj(OZ_FORCE, "bigCrate", oX, oY - 1, oZ - 3)
  ozOrbisAddObj(OZ_FORCE, "smallCrate", oX + 1.1, oY - 3, oZ - 2)
  ozOrbisAddObj(OZ_FORCE, "smallCrate", oX - 2.5, oY - 2, oZ - 3)
  ozOrbisAddObj(OZ_FORCE, "smallCrate", oX - 1, oY - 2.5, oZ - 3)

  local infoPush = ozOrbisAddObj(OZ_FORCE, "datacube", oX + 6, oY - 2, oZ - 2.5)
  local infoCarry = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 4, oY - 2.5, oZ - 2.5)
  local infoWaterPush = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 6, oY, oZ + 1.5)

  ozNirvanaAddMemo(infoPush, ozGettext(
    "6. PUSHING\n\n" ..
    "You can push around many object you see. It's also possible to push them to the side if " ..
    "you don't walk into them directly, but slightly to one side (diagonally)."))

  ozNirvanaAddMemo(infoCarry, ozGettext(
    "7. CARRYING\n\n" ..
    "You can also carry around objects that you are strong enough to lift. Press middle mouse " ..
    "button to lift/drop an object when you see the 'lift' or 'carrying' icon under your " ..
    "crosshair. To throw the object you are carrying roll your mouse wheel up.\n\n" ..
    "There are some limitations though. E.g. you cannot lift objects while climbing a ladder " ..
    "or holding a weapon. You also cannot lift somebody who is already carrying something " ..
    "and you cannot run while carrying an object.\n\n" ..
    "The object you carry will fall from your hands if you jump of if it touches some other " ..
    "object above (so you cannot carry several stacked objects).\n\n" ..
    "If somebody else carries you, you can free yourself with jumping.\n\n" ..
    "Now use the available boxes to get to the next room."))

  ozNirvanaAddMemo(infoWaterPush, ozGettext(
    "8. PUSHING IN WATER\n\n" ..
    "You will need a create from the previous room for this one. Bring it here, throw it in the " ..
    "water and push it under water to the other side. There you will need the crate to jump on " ..
    "it to climb on a ledge.\n\n" ..
    "You can also push an object upwards or downwards with side-pushing while the object is in " ..
    "the water."))

  local oX, oY, oZ = 82, -55, 69.5
  ozOrbisAddStr(OZ_FORCE, "forum", oX, oY, oZ, OZ_NORTH)

  ozOrbisAddObj(OZ_FORCE, "firstAid", oX - 4.5, oY - 3.2, oZ - 1.5)
  ozOrbisAddObj(OZ_FORCE, "firstAid", oX - 4.5, oY - 2.5, oZ - 1.5)
  ozOrbisAddObj(OZ_FORCE, "cvicek", oX - 4.5, oY - 2, oZ - 1.5)
  ozOrbisAddObj(OZ_FORCE, "metalCrate", oX + 4.5, oY - 3, oZ - 1.5)
  ozObjAddItem("firstAid")
  ozObjAddItem("galileo")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozOrbisAddObj(OZ_FORCE, "firstAid", oX - 4.5, oY + 2, oZ - 1.5)
  ozOrbisAddObj(OZ_FORCE, "firstAid", oX - 4.5, oY + 2, oZ - 1.1)
  ozOrbisAddObj(OZ_FORCE, "cvicek", oX - 4.5, oY + 2, oZ - 0.5)
  ozOrbisAddObj(OZ_FORCE, "musicPlayer", oX - 4.9, oY + 2.8, oZ - 0.5)
  ozOrbisAddObj(OZ_FORCE, "galileo", oX - 4.9, oY + 3.5, oZ - 0.5)
  ozOrbisAddObj(OZ_FORCE, "nvGoggles", oX - 4.3, oY + 2.8, oZ - 0.5)
  ozOrbisAddObj(OZ_FORCE, "binoculars", oX - 4.3, oY + 3.5, oZ - 0.5)

  local infoUI = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 1, oY - 4, oZ - 1)
  local infoItem = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 4, oY - 1, oZ - 1)
  local infoExchange = ozOrbisAddObj(OZ_FORCE, "datacube", oX + 4, oY - 1, oZ - 1)
  local infoHealth = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 4, oY + 1, oZ - 1)
  local infoSpecial = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 4, oY + 4, oZ - 1)
  local infoSaving = ozOrbisAddObj(OZ_FORCE, "datacube", oX + 4, oY + 3, oZ - 1)

  ozNirvanaAddMemo(infoUI, ozGettext(
    "9. USER INTERFACE\n\n" ..
    "Use TAB key to toggle between freelook and UI mode. In UI mode you see several other " ..
    "windows (depending on situation) and manage your inventory.\n\n" ..
    "You can still look around in UI mode by pushing mouse cursor into edges of the screen.\n\n" ..
    "Windows can be dragged around with left mouse button while holding the ALT key."))

  ozNirvanaAddMemo(infoItem, ozGettext(
    "10. INVENTORY\n\n" ..
    "If you see a blue hand icon right to your crosshair, it means you can activate the object " ..
    "with right click.\n\n" ..
    "Purple icon left to your crosshair means the item can be put into your inventory if you " ..
    "roll mouse wheel down. You can only put items into your inventory if you have enough " ..
    "room.\n\n" ..
    "By left clicking an item in you inventory interface you will drop it and with middle click " ..
    "you will start to carry it like boxes in previous tutorial. For both actions you need " ..
    "enough room in front of you.\n\n" ..
    "You can still activate items while they are in your inventory (blue hand is shown at the " ..
    "bottom of inventory window if the selected item can be activated)."))

  ozNirvanaAddMemo(infoExchange, ozGettext(
    "11. TRANSFER BETWEEN INVENTORIES\n\n" ..
    "If you see two arrows left of you crosshair, you can browse object in front of you and " ..
    "swap items between it and your inventory.\n\n" ..
    "If you roll your mouse wheel down, both inventories will be shown. With left click you can " ..
    "transfer items between the inventories.\n\n" ..
    "Sometimes inventory is to big for all slots to be shown. In such case you can scroll slots " ..
    "in the inventory windows with your mouse wheel. Arrows on top left and/or bottom left side " ..
    "are shown when the inventory can be scrolled."))

  ozNirvanaAddMemo(infoHealth, ozGettext(
    "12. HEALTH\n\n" ..
    "First aid kits are disposable, you can only use them once. If you are not seriously hurt " ..
    "it's probably best to spare first aid for a later time. There's also a bonus if you are a " ..
    "beast (which you currently are): your health regenerates slowly.\n\n" ..
    "One first aid kit restores 100 units of health. Beasts like you have 150 units of " ..
    "health.\n\n" ..
    "Your health is show in a green bar above stamina (becomes red when your health drops). If " ..
    "you are heavily wounded, you are unable to run."))

  ozNirvanaAddMemo(infoSpecial, ozGettext(
    "13. SPECIAL ITEMS\n\n" ..
    "There are also some items that give you special abilities when in your inventory. Galileo " ..
    "enables a minimap (press M while in UI mode to maximise it), music player enables a Music " ..
    "Player window while in UI mode, while night vision goggles and binoculars can be activated " ..
    "by pressing N and B keys respectively.\n\n" ..
    "Note that some types of bots and vehicles already have some of those features built-in."))

  ozNirvanaAddMemo(infoSaving, ozGettext(
    "14. SAVING AND CAMERA\n\n" ..
    "There are some dangers ahead. You can quick-save with F5 and quick-load with F7.\n\n" ..
    "Every time you exit the level, it is auto-saved. To load last auto-saved state press " ..
    "F8.\n\n" ..
    "To toggle to 3rd person view use NUMPAD ENTER and to toggle free camera rotation press " ..
    "NUMPAD *."))

  ozOrbisAddObj(OZ_FORCE, "bomb", oX + 25, oY + 17, oZ - 2)
  ozOrbisAddObj(OZ_FORCE, "bomb", oX + 25, oY + 18, oZ - 2)

  local infoBombs = ozOrbisAddObj(OZ_FORCE, "datacube", oX + 23, oY + 17.5, oZ - 1.5)

  ozNirvanaAddMemo(infoBombs, ozGettext(
    "15. BOMBS\n\n" ..
    "Some objects like this bombs are dangerous. They will explode when destroyed or 5 s after " ..
    "activation (they can also be deactivated by right clicking them again)."))

  ozOrbisAddObj(OZ_FORCE, "beast$plasmagun", oX - 5, oY + 20, oZ - 2)
  ozOrbisAddObj(OZ_FORCE, "metalBarrel", oX - 10, oY + 30, oZ)
  ozOrbisAddObj(OZ_FORCE, "metalBarrel", oX - 8, oY + 30, oZ)

  ozOrbisAddObj(OZ_FORCE, "droid", oX - 5, oY + 30, oZ, true)
  ozObjAddItem("cvicek")
  ozBotSetH(180)
  ozBotSetMind("")

  ozOrbisAddObj(OZ_FORCE, "droid.OOM-9", oX - 2.5, oY + 30, oZ, true)
  ozObjAddItem("firstAid")
  ozBotSetH(180)
  ozBotSetMind("")

  ozOrbisAddObj(OZ_FORCE, "metalCrate", oX, oY + 30, oZ - 1)

  local infoWeapon = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 6, oY + 20, oZ - 0.5)
  local infoCorpse = ozOrbisAddObj(OZ_FORCE, "datacube", oX - 6, oY + 30, oZ)

  ozNirvanaAddMemo(infoWeapon, ozGettext(
    "16. WEAPONS\n\n" ..
    "To use a weapon you first need to put in in your inventory. By right clicking it in you " ..
    "inventory interface you can arm (red fist icon) and disarm it (grey fist icon).\n\n" ..
    "There's also a shortcut: when you right click a weapon laying on the floor you will both " ..
    "put it in your inventory and arm it at the same time.\n\n" ..
    "You are only able to use weapons suitable for your bot class (e.g. if you are a beast you " ..
    "cannot use droid weapons and vice versa).\n\n" ..
    "To fire your current weapon press left mouse button.\n\n" ..
    "Test that plasma gun on objects ahead and those two droids."))

  ozNirvanaAddMemo(infoCorpse, ozGettext(
    "17. CORPSES\n\n" ..
    "Corpses can also be searched for items. Use them like the metal container before.\n\n" ..
    "But don't wait too long. Corpses disappear some time after death."))

  ozOrbisAddObj(OZ_FORCE, "hoverTank", 55, -30, 72)
  ozOrbisAddObj(OZ_FORCE, "raptor", 45, -30, 73)
  ozOrbisAddObj(OZ_FORCE, "serviceStation", 35, -30, 74)

  local infoVehicle = ozOrbisAddObj(OZ_FORCE, "datacube", 58, -33, 71)
  local infoRaptor = ozOrbisAddObj(OZ_FORCE, "datacube", 50, -35, 72)
  local infoService = ozOrbisAddObj(OZ_FORCE, "datacube", 35, -35, 73)

  ozNirvanaAddMemo(infoVehicle, ozGettext(
    "18. VEHICLES\n\n" ..
    "You can use the same keys to control vehicle as for movement.\n\n" ..
    "To switch weapons use right mouse button.\n\n" ..
    "To exit the vehicle press X and to eject press ALT + X. On destruction some vehicles " ..
    "automatically eject you."))

  ozNirvanaAddMemo(infoRaptor, ozGettext(
    "19. TRUNK\n\n" ..
    "Some vehicles also have a trunk. In that case you can swap items between your inventory " ..
    "and the vehicle as you did for that metal crate. It also works once you are in the vehicle " ..
    "(just press TAB key to open inventory window)."))

  ozNirvanaAddMemo(infoService, ozGettext(
    "20. SERVICE STATION\n\n" ..
    "You can repair and rearm vehicles at a service station. Park a vehicle close enough, go " ..
    "out and use the service station.\n\n" ..
    "Service station will also reload all weapons in your inventory. For mechanical units, it " ..
    "will also heal (repair) them and refill their stamina."))

  ozOrbisAddObj(OZ_FORCE, "goblin", 58, -52, 70)
  ozBotSetMind("")
  ozBotSetH(270)
  ozOrbisAddObj(OZ_FORCE, "droid", 58, -48, 70)
  ozBotSetMind("")
  ozBotSetH(270)

  local infoReincarnation = ozOrbisAddObj(OZ_FORCE, "datacube", 60, -50, 69.6)
  local infoQuests = ozOrbisAddObj(OZ_FORCE, "datacube", 60, -55, 69.3)

  ozNirvanaAddMemo(infoReincarnation, ozGettext(
    "21. STRATEGIC MODE\n\n" ..
    "(Read to the end before you try it.)\n\n" ..
    "You can exit the bot you are controlling by pressing I key. That puts you in strategic " ..
    "mode. You move the camera the same way you move your bot. By pressing NUMPAD ENTER you " ..
    "switch between strategic camera (top-down) and free camera. In both modes you can still " ..
    "use TAB key to toggle between user interface mode and freelook mode.\n\n" ..
    "To incarnate into a bot select it by left clicking and press I.\n\n" ..
    "Reincarnation and strategic mode are not always available. Depending on a mission, " ..
    "reincarnation can be limited to specific bots only or completely disabled (including " ..
    "strategic mode)."))

  ozNirvanaAddMemo(infoQuests, ozGettext(
    "22. OBJECTIVES\n\n" ..
    "When in the user interface mode, you see your current objective on top of the screen. By " ..
    "clicking '+' you can open or close objective description and by clicking '<' and '>' you " ..
    "cycle between all available objectives for the current mission.\n\n" ..
    "If you posses Galileo gadget, you also see your current objective position on your " ..
    "minimap."))

  ozOrbisAddObj(OZ_FORCE, "beast", 100, -100, 71)
  ozBotSetH(0)
  ozBotSetMind("beast_niceBeastie")

  ozOrbisAddObj(OZ_FORCE, "beast", 100, -50, 70)
  ozBotSetH(90)
  ozBotSetMind("beast_niceBeastie")

  ozOrbisAddObj(OZ_FORCE, "beast", 60, -80, 69)
  ozBotSetH(270)
  ozBotSetMind("beast_niceBeastie")

  ozQuestAdd(
    ozGettext("Tutorial"),
    ozGettext("Get familiar with all aspects of OpenZone game."),
    60, -46, 69.5,
    OZ_QUEST_NONE)

  floraSeed(0.0015, 12, 0.75)

  ozBindObj(me)
  ozCameraAllowReincarnation(true)
  ozCameraAddSwitchableBot(me)
  ozCameraSetBot(me)
end
