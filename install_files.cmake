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
    src/oz/Config.cpp
    src/oz/Config.hpp
    src/oz/DArray.hpp
    src/oz/DList.hpp
    src/oz/Exception.hpp
    src/oz/HashIndex.hpp
    src/oz/HashString.hpp
    src/oz/List.hpp
    src/oz/Log.cpp
    src/oz/Log.hpp
    src/oz/Map.hpp
    src/oz/Mat33.hpp
    src/oz/Mat44.hpp
    src/oz/Math.cpp
    src/oz/Math.hpp
    src/oz/Pair.hpp
    src/oz/Pool.hpp
    src/oz/Quat.hpp
    src/oz/SBitset.hpp
    src/oz/SVector.hpp
    src/oz/Sparse.hpp
    src/oz/String.cpp
    src/oz/String.hpp
    src/oz/Vec3.hpp
    src/oz/Vector.hpp
    src/oz/arrays.hpp
    src/oz/common.cpp
    src/oz/common.hpp
    src/oz/iterables.hpp
    src/oz/oz.hpp
    DESTINATION include/oz
    COMPONENT liboz )
  install( FILES ${liboz_file} DESTINATION lib COMPONENT liboz )
endif( OZ_INSTALL_LIBOZ )

string( TOUPPER "LOCATION_${CMAKE_BUILD_TYPE}" client_location_var )
get_target_property( client_file openzone ${client_location_var} )

if( WIN32 )
  install( FILES "oalinst.exe" DESTINATION support COMPONENT client )
endif( WIN32 )

if( OZ_INSTALL_CLIENT )
  install( FILES ${client_file}
    DESTINATION bin
    PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    COMPONENT client)
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
    data/maps/house.ozBSP
    data/maps/pool.ozBSP
    data/maps/test.ozBSP
    DESTINATION share/openzone/maps )

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
    data/mdl/bigCrate/crate1.jpg
    data/mdl/bigCrate/data.mtl
    data/mdl/bigCrate/data.obj
    DESTINATION share/openzone/mdl/data/mdl/bigCrate )

  install( FILES
    data/mdl/bombs/skin.jpg
    data/mdl/bombs/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/bombs )

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
    data/mdl/hobgoblin/skin.jpg
    data/mdl/hobgoblin/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/hobgoblin )

  install( FILES
    data/mdl/knight/readme.txt
    data/mdl/knight/skin.jpg
    data/mdl/knight/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/knight )

  install( FILES
    data/mdl/raptor/data.mtl
    data/mdl/raptor/data.obj
    DESTINATION share/openzone/mdl/data/mdl/raptor )

  install( FILES
    data/mdl/rifle/skin.jpg
    data/mdl/rifle/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/rifle )

  install( FILES
    data/mdl/smallCrate/crate1.jpg
    data/mdl/smallCrate/data.mtl
    data/mdl/smallCrate/data.obj
    DESTINATION share/openzone/mdl/data/mdl/smallCrate )

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
