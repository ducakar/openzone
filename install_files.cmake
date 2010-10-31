string( TOUPPER "LOCATION_${CMAKE_BUILD_TYPE}" liboz_location_var )
get_target_property( liboz_file oz ${liboz_location_var} )

if( OZ_INSTALL_LIBOZ )
  install( FILES
    ${CMAKE_BINARY_DIR}/src/oz/ozconfig.hpp
    src/oz/Alloc.cpp
    src/oz/Alloc.hpp
    src/oz/Array.hpp
    src/oz/AutoPtr.hpp
    src/oz/Bitset.hpp
    src/oz/Buffer.cpp
    src/oz/Buffer.hpp
    src/oz/Config.cpp
    src/oz/Config.hpp
    src/oz/DArray.hpp
    src/oz/DList.hpp
    src/oz/Exception.cpp
    src/oz/Exception.hpp
    src/oz/HashIndex.hpp
    src/oz/HashString.hpp
    src/oz/List.hpp
    src/oz/Log.cpp
    src/oz/Log.hpp
    src/oz/Map.hpp
    src/oz/Mat33.cpp
    src/oz/Mat33.hpp
    src/oz/Mat44.cpp
    src/oz/Mat44.hpp
    src/oz/Math.cpp
    src/oz/Math.hpp
    src/oz/Pair.hpp
    src/oz/Pool.hpp
    src/oz/Quat.cpp
    src/oz/Quat.hpp
    src/oz/SBitset.hpp
    src/oz/SVector.hpp
    src/oz/Sparse.hpp
    src/oz/String.cpp
    src/oz/String.hpp
    src/oz/Vec3.cpp
    src/oz/Vec3.hpp
    src/oz/Vector.hpp
    src/oz/arrays.hpp
    src/oz/common.cpp
    src/oz/common.hpp
    src/oz/iterables.hpp
    src/oz/oz.hpp
    src/oz/stream.hpp
    DESTINATION include/oz )
  install( FILES ${liboz_file} DESTINATION lib )
endif( OZ_INSTALL_LIBOZ )

if( WIN32 )
  install( FILES "oalinst.exe" DESTINATION support )
endif( WIN32 )

string( TOUPPER "LOCATION_${CMAKE_BUILD_TYPE}" client_location_var )
get_target_property( client_file openzone ${client_location_var} )

if( OZ_INSTALL_CLIENT )
  install( FILES ${client_file}
    DESTINATION bin
    PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE )
endif( OZ_INSTALL_CLIENT )

if( OZ_INSTALL_DATA )

  install( FILES
    data/class/Beast.rc
    data/class/BigCrate.rc
    data/class/Bomb.rc
    data/class/Droid.rc
    data/class/DroidCommander.rc
    data/class/Explosion.rc
    data/class/FirstAid.rc
    data/class/Goblin.rc
    data/class/Knight.rc
    data/class/Lara.rc
    data/class/Lord.rc
    data/class/MetalBarrel.rc
    data/class/Plant1.rc
    data/class/Plant2.rc
    data/class/Plant3.rc
    data/class/Raptor.rc
    data/class/Rifle.rc
    data/class/SmallCrate.rc
    data/class/Tree1.rc
    data/class/Tree2.rc
    data/class/Tree3.rc
    data/class/Wine.rc
    data/class/WoodBarrel.rc
    DESTINATION share/openzone/class )

  install( FILES
    data/cursors/X_cursor.png
    data/cursors/fleur.png
    data/cursors/hand2.png
    data/cursors/left_ptr.png
    data/cursors/xterm.png
    DESTINATION share/openzone/cursors )

  install( FILES
    data/fonts/DejaVuSans.ttf
    data/fonts/DejaVuSansMono.ttf
    DESTINATION share/openzone/fonts )

  install( FILES
    data/lua/matrix/handlers.lua
    DESTINATION share/openzone/lua/matrix )

  install( FILES
    data/lua/nirvana/droid.lua
    data/lua/nirvana/minds.lua
    DESTINATION share/openzone/lua/nirvana )

  install( FILES
    data/maps/castle.ozBSP
    data/maps/center.ozBSP
    data/maps/door.ozBSP
    data/maps/house.ozBSP
    data/maps/pool.ozBSP
    data/maps/rod.ozBSP
    data/maps/test.ozBSP
    DESTINATION share/openzone/maps )

  install( FILES
    data/mdl/MarvinMartian/MarvinMartian.txt
    data/mdl/MarvinMartian/a_grenades.md2
    data/mdl/MarvinMartian/a_m61frag.md2
    data/mdl/MarvinMartian/skin.jpg
    data/mdl/MarvinMartian/tris.md2
    data/mdl/MarvinMartian/w_akimbo.md2
    data/mdl/MarvinMartian/w_bfg.md2
    data/mdl/MarvinMartian/w_cannon.md2
    data/mdl/MarvinMartian/w_chaingun.md2
    data/mdl/MarvinMartian/w_glauncher.md2
    data/mdl/MarvinMartian/w_hyperblaster.md2
    data/mdl/MarvinMartian/w_knife.md2
    data/mdl/MarvinMartian/w_m4.md2
    data/mdl/MarvinMartian/w_machinegun.md2
    data/mdl/MarvinMartian/w_mk23.md2
    data/mdl/MarvinMartian/w_mp5.md2
    data/mdl/MarvinMartian/w_railgun.md2
    data/mdl/MarvinMartian/w_rlauncher.md2
    data/mdl/MarvinMartian/w_shotgun.md2
    data/mdl/MarvinMartian/w_sniper.md2
    data/mdl/MarvinMartian/w_sshotgun.md2
    data/mdl/MarvinMartian/w_super90.md2
    data/mdl/MarvinMartian/weapon.md2
    DESTINATION share/openzone/mdl/data/mdl/MarvinMartian )

  install( FILES
    data/mdl/barrel1/readme.txt
    data/mdl/barrel1/skin.jpg
    data/mdl/barrel1/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/barrel1 )

  install( FILES
    data/mdl/bauul/bauul.txt
    data/mdl/bauul/skin.jpg
    data/mdl/bauul/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/bauul )

  install( FILES
    data/mdl/bdroid/00M-9.jpg
    data/mdl/bdroid/bdroid.txt
    data/mdl/bdroid/infantry.jpg
    data/mdl/bdroid/pilot.jpg
    data/mdl/bdroid/security.jpg
    data/mdl/bdroid/skin.jpg
    data/mdl/bdroid/tris.md2
    data/mdl/bdroid/weapon.jpg
    data/mdl/bdroid/weapon.md2
    DESTINATION share/openzone/mdl/data/mdl/bdroid )

  install( FILES
    data/mdl/bigCrate/crate1.jpg
    data/mdl/bigCrate/data.mtl
    data/mdl/bigCrate/data.obj
    DESTINATION share/openzone/mdl/data/mdl/bigCrate )

  install( FILES
    data/mdl/bombs/skin.jpg
    data/mdl/bombs/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/bombs )

  install( FILES
    data/mdl/droid/blasted.jpg
    data/mdl/droid/ctf_b.jpg
    data/mdl/droid/ctf_r.jpg
    data/mdl/droid/dirty.jpg
    data/mdl/droid/droid-sounds.txt
    data/mdl/droid/droid.jpg
    data/mdl/droid/droid.txt
    data/mdl/droid/oom-9.jpg
    data/mdl/droid/skin.jpg
    data/mdl/droid/slashed.jpg
    data/mdl/droid/tris.md2
    data/mdl/droid/w_bfg.md2
    data/mdl/droid/w_blaster.md2
    data/mdl/droid/w_chaingun.md2
    data/mdl/droid/w_glauncher.md2
    data/mdl/droid/w_grapple.md2
    data/mdl/droid/w_hyperblaster.md2
    data/mdl/droid/w_machinegun.md2
    data/mdl/droid/w_railgun.md2
    data/mdl/droid/w_rlauncher.md2
    data/mdl/droid/w_shotgun.md2
    data/mdl/droid/w_sshotgun.md2
    data/mdl/droid/weapon.md2
    DESTINATION share/openzone/mdl/data/mdl/droid )

  install( FILES
    data/mdl/droid_commander/bdroid.txt
    data/mdl/droid_commander/skin.jpg
    data/mdl/droid_commander/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/droid_commander )

  install( FILES
    data/mdl/droid_infantry/bdroid.txt
    data/mdl/droid_infantry/skin.jpg
    data/mdl/droid_infantry/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/droid_infantry )

  install( FILES
    data/mdl/goblin/readme.txt
    data/mdl/goblin/skin.jpg
    data/mdl/goblin/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/goblin )

  install( FILES
    data/mdl/health/data.mtl
    data/mdl/health/data.obj
    data/mdl/health/health.png
    DESTINATION share/openzone/mdl/data/mdl/health )

  install( FILES
    data/mdl/hellpig/readme.txt
    data/mdl/hellpig/skin.jpg
    data/mdl/hellpig/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/hellpig )

  install( FILES
    data/mdl/hobgoblin/skin.jpg
    data/mdl/hobgoblin/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/hobgoblin )

  install( FILES
    data/mdl/ivy/skin.jpg
    data/mdl/ivy/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/ivy )

  install( FILES
    data/mdl/knight/readme.txt
    data/mdl/knight/skin.jpg
    data/mdl/knight/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/knight )

  install( FILES
    data/mdl/lara/LaraCroft - ReadMe.txt
    DESTINATION share/openzone/mdl/data/mdl/lara )

  install( FILES
    data/mdl/leafs/skin.jpg
    data/mdl/leafs/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/leafs )

  install( FILES
    data/mdl/monkey/Osama.jpg
    data/mdl/monkey/data.mtl
    data/mdl/monkey/data.obj
    DESTINATION share/openzone/mdl/data/mdl/monkey )

  install( FILES
    data/mdl/mushroom/skin.jpg
    data/mdl/mushroom/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/mushroom )

  install( FILES
    data/mdl/ogro/Ogro.txt
    data/mdl/ogro/skin.jpg
    data/mdl/ogro/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/ogro )

  install( FILES
    data/mdl/palmtree/skin.jpg
    data/mdl/palmtree/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/palmtree )

  install( FILES
    data/mdl/pirate/pirate.mtl
    data/mdl/pirate/pirate.obj
    DESTINATION share/openzone/mdl/data/mdl/pirate )

  install( FILES
    data/mdl/raptor/data.mtl
    data/mdl/raptor/data.obj
    DESTINATION share/openzone/mdl/data/mdl/raptor )

  install( FILES
    data/mdl/rat/Ratamahatta.txt
    data/mdl/rat/skin.jpg
    data/mdl/rat/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/rat )

  install( FILES
    data/mdl/reaper/blue.jpg
    data/mdl/reaper/eye_glow.jpg
    data/mdl/reaper/reaper.jpg
    data/mdl/reaper/red.jpg
    data/mdl/reaper/spec.jpg
    DESTINATION share/openzone/mdl/data/mdl/reaper )

  install( FILES
    data/mdl/rhino/rhino.txt
    data/mdl/rhino/skin.jpg
    data/mdl/rhino/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/rhino )

  install( FILES
    data/mdl/rifle/skin.jpg
    data/mdl/rifle/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/rifle )

  install( FILES
    data/mdl/slith/skin.jpg
    data/mdl/slith/slithMe.txt
    data/mdl/slith/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/slith )

  install( FILES
    data/mdl/smallCrate/crate1.jpg
    data/mdl/smallCrate/data.mtl
    data/mdl/smallCrate/data.obj
    DESTINATION share/openzone/mdl/data/mdl/smallCrate )

  install( FILES
    data/mdl/smplant/skin.jpg
    data/mdl/smplant/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/smplant )

  install( FILES
    data/mdl/tree1/skin.jpg
    data/mdl/tree1/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/tree1 )

  install( FILES
    data/mdl/tree2/readme.txt
    data/mdl/tree2/skin.jpg
    data/mdl/tree2/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/tree2 )

  install( FILES
    data/mdl/tree3/readme.txt
    data/mdl/tree3/skin.jpg
    data/mdl/tree3/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/tree3 )

  install( FILES
    data/mdl/winebottle/skin.jpg
    data/mdl/winebottle/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/winebottle )

  install( FILES
    data/name/names
    DESTINATION share/openzone/name )

  install( FILES
    data/snd/cg2.ogg
    data/snd/damage_bot1.ogg
    data/snd/damage_bot2.ogg
    data/snd/death1.ogg
    data/snd/death2.ogg
    data/snd/destroy_metal1.ogg
    data/snd/destroy_wood1.ogg
    data/snd/explosion.ogg
    data/snd/friction1.ogg
    data/snd/gesture_flip.ogg
    data/snd/hit1.ogg
    data/snd/hit_bot1.ogg
    data/snd/hit_bot2.ogg
    data/snd/hit_metal1.ogg
    data/snd/hit_metal2.ogg
    data/snd/hit_wood1.ogg
    data/snd/hit_wood2.ogg
    data/snd/jump1.ogg
    data/snd/jump2.ogg
    data/snd/land1.ogg
    data/snd/land2.ogg
    data/snd/outofammo.ogg
    data/snd/splash1.ogg
    data/snd/splash2.ogg
    data/snd/tick.ogg
    DESTINATION share/openzone/snd )

  install( FILES
    data/terra/heightmap.ozTerra
    DESTINATION share/openzone/terra )

  install( FILES
    data/textures/oz/README
    data/textures/oz/_Drkalisce.png
    data/textures/oz/_Samostojeca_voda.png
    data/textures/oz/crate1.jpg
    data/textures/oz/explosion.jpg
    data/textures/oz/particle.png
    data/textures/oz/roof1.jpg
    data/textures/oz/slick.jpg
    data/textures/oz/stone1.jpg
    data/textures/oz/stone2.jpg
    data/textures/oz/stone3.jpg
    data/textures/oz/water1.jpg
    data/textures/oz/wood1.jpg
    data/textures/oz/wood2.jpg
    DESTINATION share/openzone/textures/oz )

  install( FILES
    data/ui/crosshair.png
    data/ui/grab.png
    data/ui/mount.png
    data/ui/take.png
    data/ui/use.png
    DESTINATION share/openzone/ui )

endif( OZ_INSTALL_DATA )
