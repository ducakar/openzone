string( TOUPPER "LOCATION_${CMAKE_BUILD_TYPE}" liboz_location_var )
get_target_property( liboz_file oz ${liboz_location_var} )

if( OZ_INSTALL_LIBOZ )
  install( FILES
    ${CMAKE_BINARY_DIR}/src/oz/ozconfig.hpp
    src/oz/Alloc.cpp
    src/oz/Alloc.hpp
    src/oz/Array.hpp
    src/oz/Bitset.hpp
    src/oz/Buffer.cpp
    src/oz/Buffer.hpp
    src/oz/Config.cpp
    src/oz/Config.hpp
    src/oz/DArray.hpp
    src/oz/DList.hpp
    src/oz/Directory.cpp
    src/oz/Directory.hpp
    src/oz/Exception.cpp
    src/oz/Exception.hpp
    src/oz/HashIndex.hpp
    src/oz/HashString.hpp
    src/oz/List.hpp
    src/oz/Log.cpp
    src/oz/Log.hpp
    src/oz/Map.hpp
    src/oz/Mat44.cpp
    src/oz/Mat44.hpp
    src/oz/Math.cpp
    src/oz/Math.hpp
    src/oz/Pair.hpp
    src/oz/Plane.hpp
    src/oz/Point3.cpp
    src/oz/Point3.hpp
    src/oz/Pool.hpp
    src/oz/Quat.cpp
    src/oz/Quat.hpp
    src/oz/SBitset.hpp
    src/oz/SVector.hpp
    src/oz/Sparse.hpp
    src/oz/String.cpp
    src/oz/String.hpp
    src/oz/System.cpp
    src/oz/System.hpp
    src/oz/Vec3.cpp
    src/oz/Vec3.hpp
    src/oz/Vector.hpp
    src/oz/arrays.hpp
    src/oz/common.cpp
    src/oz/common.hpp
    src/oz/iterables.hpp
    src/oz/oz.cpp
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
    DESTINATION share/openzone/class )

  install( FILES
    DESTINATION share/openzone/cursors )

  install( FILES
    data/fonts/COPYING.DejaVu
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
    DESTINATION share/openzone/maps )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/DISABLED )

  install( FILES
    data/mdl/barrel1/readme.txt
    data/mdl/barrel1/skin.jpg
    data/mdl/barrel1/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/barrel1 )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/barrel1.ozcSMM )

  install( FILES
    data/mdl/bauul/bauul.txt
    data/mdl/bauul/skin.jpg
    data/mdl/bauul/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/bauul )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/bauul.ozcMD2 )

  install( FILES
    data/mdl/bigCrate/crate1.jpg
    data/mdl/bigCrate/data.mtl
    data/mdl/bigCrate/data.obj
    DESTINATION share/openzone/mdl/data/mdl/bigCrate )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/bigCrate.ozcSMM )

  install( FILES
    data/mdl/bombs/skin.jpg
    data/mdl/bombs/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/bombs )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/bombs.ozcSMM )

  install( FILES
    data/mdl/droid_commander/bdroid.txt
    data/mdl/droid_commander/skin.jpg
    data/mdl/droid_commander/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/droid_commander )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/droid_commander.ozcMD2 )

  install( FILES
    data/mdl/droid_infantry/bdroid.txt
    data/mdl/droid_infantry/skin.jpg
    data/mdl/droid_infantry/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/droid_infantry )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/droid_infantry.ozcMD2 )

  install( FILES
    data/mdl/goblin/readme.txt
    data/mdl/goblin/skin.jpg
    data/mdl/goblin/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/goblin )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/goblin.ozcMD2 )

  install( FILES
    data/mdl/health/data.mtl
    data/mdl/health/data.obj
    data/mdl/health/health.png
    DESTINATION share/openzone/mdl/data/mdl/health )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/health.ozcSMM )

  install( FILES
    data/mdl/hobgoblin/skin.jpg
    data/mdl/hobgoblin/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/hobgoblin )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/hobgoblin.ozcMD2 )

  install( FILES
    data/mdl/ivy/skin.jpg
    data/mdl/ivy/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/ivy )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/ivy.ozcSMM )

  install( FILES
    data/mdl/knight/readme.txt
    data/mdl/knight/skin.jpg
    data/mdl/knight/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/knight )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/knight.ozcMD2 )

  install( FILES
    data/mdl/leafs/skin.jpg
    data/mdl/leafs/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/leafs )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/leafs.ozcSMM )

  install( FILES
    data/mdl/mushroom/skin.jpg
    data/mdl/mushroom/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/mushroom )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/mushroom.ozcSMM )

  install( FILES
    data/mdl/palmtree/skin.jpg
    data/mdl/palmtree/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/palmtree )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/palmtree.ozcSMM )

  install( FILES
    data/mdl/raptor/data.mtl
    data/mdl/raptor/data.obj
    DESTINATION share/openzone/mdl/data/mdl/raptor )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/raptor.ozcSMM )

  install( FILES
    data/mdl/rifle/skin.jpg
    data/mdl/rifle/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/rifle )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/rifle.ozcMD2 )

  install( FILES
    data/mdl/smallCrate/crate1.jpg
    data/mdl/smallCrate/data.mtl
    data/mdl/smallCrate/data.obj
    DESTINATION share/openzone/mdl/data/mdl/smallCrate )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/smallCrate.ozcSMM )

  install( FILES
    data/mdl/smplant/skin.jpg
    data/mdl/smplant/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/smplant )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/smplant.ozcSMM )

  install( FILES
    data/mdl/tree1/skin.jpg
    data/mdl/tree1/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/tree1 )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/tree1.ozcSMM )

  install( FILES
    data/mdl/tree2/readme.txt
    data/mdl/tree2/skin.jpg
    data/mdl/tree2/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/tree2 )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/tree2.ozcSMM )

  install( FILES
    data/mdl/tree3/readme.txt
    data/mdl/tree3/skin.jpg
    data/mdl/tree3/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/tree3 )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/tree3.ozcSMM )

  install( FILES
    data/mdl/winebottle/skin.jpg
    data/mdl/winebottle/tris.md2
    DESTINATION share/openzone/mdl/data/mdl/winebottle )

  install( FILES
    DESTINATION share/openzone/mdl/data/mdl/winebottle.ozcSMM )

  install( FILES
    data/name/COPYING.wesnoth
    data/name/wesnoth.drake-male.txt
    DESTINATION share/openzone/name )

  install( FILES
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
    data/ui/COPYING
    data/ui/X_cursor.png
    data/ui/crosshair.png
    data/ui/fleur.png
    data/ui/grab.png
    data/ui/hand2.png
    data/ui/left_ptr.png
    data/ui/lift.png
    data/ui/mount.png
    data/ui/take.png
    data/ui/use.png
    data/ui/xterm.png
    DESTINATION share/openzone/ui )

endif( OZ_INSTALL_DATA )
