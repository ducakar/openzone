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
    src/oz/Vec4.cpp
    src/oz/Vec4.hpp
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
  install( FILES
    AUTHORS
    COPYING
    README
    DESTINATION . )
else( WIN32 )
  install( FILES
    AUTHORS
    COPYING
    README
    DESTINATION share/doc/openzone )
endif( WIN32 )

if( WIN32 )
  install( FILES
    mingw32/SDL.dll
    mingw32/SDL_ttf.dll
    mingw32/libalut-0.dll
    mingw32/libcharset-1.dll
    mingw32/libfreetype-6.dll
    mingw32/libgcc_s_sjlj-1.dll
    mingw32/libiconv-2.dll
    mingw32/libogg-0.dll
    mingw32/libstdc++-6.dll
    mingw32/libvorbis-0.dll
    mingw32/libvorbisenc-2.dll
    mingw32/libvorbisfile-3.dll
    mingw32/oalinst.exe
    DESTINATION bin )
  install( FILES
    mingw32/install_openal.bat
    mingw32/openzone-restart-world.bat
    mingw32/openzone.bat
    DESTINATION . )
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
    share/openzone/class/beast.rc
    share/openzone/class/bigCrate.rc
    share/openzone/class/bigExplosion.rc
    share/openzone/class/bomb.rc
    share/openzone/class/cvicek.rc
    share/openzone/class/drevo.rc
    share/openzone/class/droid.rc
    share/openzone/class/droidCommander.rc
    share/openzone/class/droidRifle.rc
    share/openzone/class/firstAid.rc
    share/openzone/class/goblin.rc
    share/openzone/class/goblinAxe.rc
    share/openzone/class/knight.rc
    share/openzone/class/lord.rc
    share/openzone/class/metalBarrel.rc
    share/openzone/class/palma.rc
    share/openzone/class/raptor.rc
    share/openzone/class/serviceStation.rc
    share/openzone/class/shell.rc
    share/openzone/class/smallCrate.rc
    share/openzone/class/smallExplosion.rc
    share/openzone/class/smreka.rc
    share/openzone/class/tank.rc
    DESTINATION share/openzone/class )

  install( FILES
    DESTINATION share/openzone/fonts )

  install( FILES
    share/openzone/lua/matrix/handlers.lua
    DESTINATION share/openzone/lua/matrix )

  install( FILES
    share/openzone/lua/nirvana/droid.lua
    share/openzone/lua/nirvana/minds.lua
    DESTINATION share/openzone/lua/nirvana )

  install( FILES
    share/openzone/bsp/bunker.ozBSP
    share/openzone/bsp/bunker.ozcBSP
    share/openzone/bsp/castle.ozBSP
    share/openzone/bsp/castle.ozcBSP
    share/openzone/bsp/pool.ozBSP
    share/openzone/bsp/pool.ozcBSP
    DESTINATION share/openzone/bsp )

  install( FILES
    share/openzone/bsp/tex/_Drkalisce.ozcTex
    share/openzone/bsp/tex/_Potiskalnica.ozcTex
    share/openzone/bsp/tex/_Samostojeca_voda.ozcTex
    share/openzone/bsp/tex/_Spestalnica.ozcTex
    share/openzone/bsp/tex/crate1.ozcTex
    share/openzone/bsp/tex/glass.ozcTex
    share/openzone/bsp/tex/roof1.ozcTex
    share/openzone/bsp/tex/slick.ozcTex
    share/openzone/bsp/tex/stone1.ozcTex
    share/openzone/bsp/tex/stone2.ozcTex
    share/openzone/bsp/tex/stone3.ozcTex
    share/openzone/bsp/tex/water1.ozcTex
    share/openzone/bsp/tex/wood1.ozcTex
    share/openzone/bsp/tex/wood2.ozcTex
    DESTINATION share/openzone/bsp/tex )

  install( FILES
    share/openzone/mdl/barrel1.README
    share/openzone/mdl/barrel1.ozcSMM
    share/openzone/mdl/bauul.README
    share/openzone/mdl/bauul.ozcMD2
    share/openzone/mdl/bigCrate.ozcSMM
    share/openzone/mdl/biotank.ozcSMM
    share/openzone/mdl/bombs.ozcSMM
    share/openzone/mdl/console.ozcSMM
    share/openzone/mdl/droid-rifle.README
    share/openzone/mdl/droid-rifle.ozcMD2
    share/openzone/mdl/droid_commander.README
    share/openzone/mdl/droid_commander.ozcMD2
    share/openzone/mdl/droid_infantry.README
    share/openzone/mdl/droid_infantry.ozcMD2
    share/openzone/mdl/explosion.ozcSMM
    share/openzone/mdl/goblin-axe.README
    share/openzone/mdl/goblin-axe.ozcMD2
    share/openzone/mdl/goblin.README
    share/openzone/mdl/goblin.ozcMD2
    share/openzone/mdl/health.ozcSMM
    share/openzone/mdl/hobgoblin.ozcMD2
    share/openzone/mdl/knight.README
    share/openzone/mdl/knight.ozcMD2
    share/openzone/mdl/palmtree.ozcSMM
    share/openzone/mdl/raptor.ozcSMM
    share/openzone/mdl/shell.ozcSMM
    share/openzone/mdl/smallCrate.ozcSMM
    share/openzone/mdl/tank.ozcSMM
    share/openzone/mdl/tree2.README
    share/openzone/mdl/tree2.ozcSMM
    share/openzone/mdl/tree3.README
    share/openzone/mdl/tree3.ozcSMM
    share/openzone/mdl/winebottle.ozcSMM
    DESTINATION share/openzone/mdl )

  install( FILES
    share/openzone/name/bogovi.txt
    share/openzone/name/wesnoth.COPYING
    share/openzone/name/wesnoth.drake-female.txt
    share/openzone/name/wesnoth.drake-male.txt
    share/openzone/name/wesnoth.dwarvish-male.txt
    share/openzone/name/wesnoth.elvish-female.txt
    share/openzone/name/wesnoth.elvish-male.txt
    share/openzone/name/wesnoth.gryphon-female.txt
    share/openzone/name/wesnoth.gryphon-male.txt
    share/openzone/name/wesnoth.human-female.txt
    share/openzone/name/wesnoth.human-male.txt
    share/openzone/name/wesnoth.lizard-male.txt
    share/openzone/name/wesnoth.merman-female.txt
    share/openzone/name/wesnoth.merman-male.txt
    share/openzone/name/wesnoth.naga-female.txt
    share/openzone/name/wesnoth.naga-male.txt
    share/openzone/name/wesnoth.ogre-male.txt
    share/openzone/name/wesnoth.orcish-male.txt
    share/openzone/name/wesnoth.troll-male.txt
    share/openzone/name/wesnoth.village.txt
    share/openzone/name/wesnoth.wose-male.txt
    DESTINATION share/openzone/name )

  install( FILES
    share/openzone/snd/acid5.wav
    share/openzone/snd/cg2.wav
    share/openzone/snd/damage_bot1.wav
    share/openzone/snd/damage_bot2.wav
    share/openzone/snd/death1.wav
    share/openzone/snd/death2.wav
    share/openzone/snd/destroy_metal1.wav
    share/openzone/snd/destroy_wood1.wav
    share/openzone/snd/door.wav
    share/openzone/snd/explosion.wav
    share/openzone/snd/fan-stari1.wav
    share/openzone/snd/fan.wav
    share/openzone/snd/friction1.wav
    share/openzone/snd/gesture_flip.wav
    share/openzone/snd/hit1.wav
    share/openzone/snd/hit_bot1.wav
    share/openzone/snd/hit_bot2.wav
    share/openzone/snd/hit_metal1.wav
    share/openzone/snd/hit_metal2.wav
    share/openzone/snd/hit_wood1.wav
    share/openzone/snd/hit_wood2.wav
    share/openzone/snd/hum6.wav
    share/openzone/snd/itemback.wav
    share/openzone/snd/itempick.wav
    share/openzone/snd/jump1.wav
    share/openzone/snd/jump2.wav
    share/openzone/snd/land1.wav
    share/openzone/snd/land2.wav
    share/openzone/snd/outofammo.wav
    share/openzone/snd/splash1.wav
    share/openzone/snd/splash2.wav
    share/openzone/snd/tak.wav
    share/openzone/snd/tick.wav
    DESTINATION share/openzone/snd )

  install( FILES
    share/openzone/terra/heightmap.ozTerra
    share/openzone/terra/heightmap.ozcTerra
    DESTINATION share/openzone/terra )

  install( FILES
    share/openzone/sky/sky.ozcSky
    DESTINATION share/openzone/sky )

  install( FILES
    share/openzone/ui/iconcrosshair.ozcTex
    share/openzone/ui/icongrab.ozcTex
    share/openzone/ui/iconlift.ozcTex
    share/openzone/ui/iconmount.ozcTex
    share/openzone/ui/icontake.ozcTex
    share/openzone/ui/iconuse.ozcTex
    DESTINATION share/openzone/ui )

  install( FILES
    share/openzone/glsl/COPYING
    share/openzone/glsl/bigMesh.frag
    share/openzone/glsl/bigMesh.vert
    share/openzone/glsl/celestial.frag
    share/openzone/glsl/celestial.vert
    share/openzone/glsl/header.glsl
    share/openzone/glsl/md2.frag
    share/openzone/glsl/md2.vert
    share/openzone/glsl/mesh.frag
    share/openzone/glsl/mesh.vert
    share/openzone/glsl/particles.frag
    share/openzone/glsl/particles.vert
    share/openzone/glsl/plant.frag
    share/openzone/glsl/plant.vert
    share/openzone/glsl/simple.frag
    share/openzone/glsl/simple.vert
    share/openzone/glsl/stars.frag
    share/openzone/glsl/stars.vert
    share/openzone/glsl/submergedTerraLand.frag
    share/openzone/glsl/submergedTerraLand.vert
    share/openzone/glsl/submergedTerraWater.frag
    share/openzone/glsl/submergedTerraWater.vert
    share/openzone/glsl/terraLand.frag
    share/openzone/glsl/terraLand.vert
    share/openzone/glsl/terraWater.frag
    share/openzone/glsl/terraWater.vert
    share/openzone/glsl/ui.frag
    share/openzone/glsl/ui.vert
    DESTINATION share/openzone/glsl )

endif( OZ_INSTALL_DATA )
