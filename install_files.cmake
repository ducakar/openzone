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
    share/openzone/data/class/beast.rc
    share/openzone/data/class/bigCrate.rc
    share/openzone/data/class/bigExplosion.rc
    share/openzone/data/class/bomb.rc
    share/openzone/data/class/cvicek.rc
    share/openzone/data/class/drevo.rc
    share/openzone/data/class/droid.rc
    share/openzone/data/class/droidCommander.rc
    share/openzone/data/class/droidRifle.rc
    share/openzone/data/class/firstAid.rc
    share/openzone/data/class/goblin.rc
    share/openzone/data/class/goblinAxe.rc
    share/openzone/data/class/knight.rc
    share/openzone/data/class/lord.rc
    share/openzone/data/class/metalBarrel.rc
    share/openzone/data/class/palma.rc
    share/openzone/data/class/raptor.rc
    share/openzone/data/class/serviceStation.rc
    share/openzone/data/class/shell.rc
    share/openzone/data/class/smallCrate.rc
    share/openzone/data/class/smallExplosion.rc
    share/openzone/data/class/smreka.rc
    share/openzone/data/class/tank.rc
    DESTINATION share/openzone/data/class )

  install( FILES
    share/openzone/data/fonts/DejaVu.COPYING
    share/openzone/data/fonts/DejaVuSans.ttf
    share/openzone/data/fonts/DejaVuSansMono.ttf
    DESTINATION share/openzone/data/fonts )

  install( FILES
    share/openzone/data/lua/matrix/handlers.lua
    DESTINATION share/openzone/data/lua/matrix )

  install( FILES
    share/openzone/data/lua/nirvana/droid.lua
    share/openzone/data/lua/nirvana/minds.lua
    DESTINATION share/openzone/data/lua/nirvana )

  install( FILES
    share/openzone/data/bsp/bunker.ozBSP
    share/openzone/data/bsp/bunker.ozcBSP
    share/openzone/data/bsp/castle.ozBSP
    share/openzone/data/bsp/castle.ozcBSP
    share/openzone/data/bsp/pool.ozBSP
    share/openzone/data/bsp/pool.ozcBSP
    DESTINATION share/openzone/data/bsp )

  install( FILES
    share/openzone/data/bsp/tex/_Drkalisce.ozcTex
    share/openzone/data/bsp/tex/_Potiskalnica.ozcTex
    share/openzone/data/bsp/tex/_Samostojeca_voda.ozcTex
    share/openzone/data/bsp/tex/_Spestalnica.ozcTex
    share/openzone/data/bsp/tex/crate1.ozcTex
    share/openzone/data/bsp/tex/glass.ozcTex
    share/openzone/data/bsp/tex/roof1.ozcTex
    share/openzone/data/bsp/tex/slick.ozcTex
    share/openzone/data/bsp/tex/stone1.ozcTex
    share/openzone/data/bsp/tex/stone2.ozcTex
    share/openzone/data/bsp/tex/stone3.ozcTex
    share/openzone/data/bsp/tex/water1.ozcTex
    share/openzone/data/bsp/tex/wood1.ozcTex
    share/openzone/data/bsp/tex/wood2.ozcTex
    DESTINATION share/openzone/data/bsp/tex )

  install( FILES
    share/openzone/data/mdl/barrel1.ozcSMM
    share/openzone/data/mdl/bauul.ozcMD2
    share/openzone/data/mdl/bigCrate.ozcSMM
    share/openzone/data/mdl/biotank.ozcSMM
    share/openzone/data/mdl/bombs.ozcSMM
    share/openzone/data/mdl/console.ozcSMM
    share/openzone/data/mdl/droid-rifle.ozcMD2
    share/openzone/data/mdl/droid_commander.ozcMD2
    share/openzone/data/mdl/droid_infantry.ozcMD2
    share/openzone/data/mdl/explosion.ozcSMM
    share/openzone/data/mdl/goblin-axe.ozcMD2
    share/openzone/data/mdl/goblin.ozcMD2
    share/openzone/data/mdl/health.ozcSMM
    share/openzone/data/mdl/hobgoblin.ozcMD2
    share/openzone/data/mdl/knight.ozcMD2
    share/openzone/data/mdl/palmtree.ozcSMM
    share/openzone/data/mdl/raptor.ozcSMM
    share/openzone/data/mdl/shell.ozcSMM
    share/openzone/data/mdl/smallCrate.ozcSMM
    share/openzone/data/mdl/tank.ozcSMM
    share/openzone/data/mdl/tree2.ozcSMM
    share/openzone/data/mdl/tree3.ozcSMM
    share/openzone/data/mdl/winebottle.ozcSMM
    DESTINATION share/openzone/data/mdl )

  install( FILES
    share/openzone/data/name/COPYING.wesnoth
    share/openzone/data/name/wesnoth.drake-male.txt
    DESTINATION share/openzone/data/name )

  install( FILES
    share/openzone/data/snd/acid5.wav
    share/openzone/data/snd/cg2.wav
    share/openzone/data/snd/damage_bot1.wav
    share/openzone/data/snd/damage_bot2.wav
    share/openzone/data/snd/death1.wav
    share/openzone/data/snd/death2.wav
    share/openzone/data/snd/destroy_metal1.wav
    share/openzone/data/snd/destroy_wood1.wav
    share/openzone/data/snd/door.wav
    share/openzone/data/snd/explosion.wav
    share/openzone/data/snd/fan-stari1.wav
    share/openzone/data/snd/fan.wav
    share/openzone/data/snd/friction1.wav
    share/openzone/data/snd/gesture_flip.wav
    share/openzone/data/snd/hit1.wav
    share/openzone/data/snd/hit_bot1.wav
    share/openzone/data/snd/hit_bot2.wav
    share/openzone/data/snd/hit_metal1.wav
    share/openzone/data/snd/hit_metal2.wav
    share/openzone/data/snd/hit_wood1.wav
    share/openzone/data/snd/hit_wood2.wav
    share/openzone/data/snd/hum6.wav
    share/openzone/data/snd/itemback.wav
    share/openzone/data/snd/itempick.wav
    share/openzone/data/snd/jump1.wav
    share/openzone/data/snd/jump2.wav
    share/openzone/data/snd/land1.wav
    share/openzone/data/snd/land2.wav
    share/openzone/data/snd/outofammo.wav
    share/openzone/data/snd/splash1.wav
    share/openzone/data/snd/splash2.wav
    share/openzone/data/snd/tak.wav
    share/openzone/data/snd/tick.wav
    DESTINATION share/openzone/data/snd )

  install( FILES
    share/openzone/data/terra/heightmap.ozTerra
    share/openzone/data/terra/heightmap.ozcTerra
    DESTINATION share/openzone/data/terra )

  install( FILES
    share/openzone/data/sky/sky.ozcSky
    DESTINATION share/openzone/data/sky )

  install( FILES
    share/openzone/data/ui/COPYING
    share/openzone/data/ui/X_cursor.ozcCur
    share/openzone/data/ui/X_cursor.ozcTex
    share/openzone/data/ui/crosshair.ozcTex
    share/openzone/data/ui/fleur.ozcCur
    share/openzone/data/ui/fleur.ozcTex
    share/openzone/data/ui/grab.ozcTex
    share/openzone/data/ui/hand2.ozcCur
    share/openzone/data/ui/hand2.ozcTex
    share/openzone/data/ui/left_ptr.ozcCur
    share/openzone/data/ui/left_ptr.ozcTex
    share/openzone/data/ui/lift.ozcTex
    share/openzone/data/ui/mount.ozcTex
    share/openzone/data/ui/take.ozcTex
    share/openzone/data/ui/use.ozcTex
    share/openzone/data/ui/xterm.ozcCur
    share/openzone/data/ui/xterm.ozcTex
    DESTINATION share/openzone/data/ui )

  install( FILES
    share/openzone/data/glsl/bigMesh.frag
    share/openzone/data/glsl/bigMesh.vert
    share/openzone/data/glsl/celestial.frag
    share/openzone/data/glsl/celestial.vert
    share/openzone/data/glsl/header.glsl
    share/openzone/data/glsl/md2.frag
    share/openzone/data/glsl/md2.vert
    share/openzone/data/glsl/mesh.frag
    share/openzone/data/glsl/mesh.vert
    share/openzone/data/glsl/particles.frag
    share/openzone/data/glsl/particles.vert
    share/openzone/data/glsl/plant.frag
    share/openzone/data/glsl/plant.vert
    share/openzone/data/glsl/simple.frag
    share/openzone/data/glsl/simple.vert
    share/openzone/data/glsl/stars.frag
    share/openzone/data/glsl/stars.vert
    share/openzone/data/glsl/submergedTerraLand.frag
    share/openzone/data/glsl/submergedTerraLand.vert
    share/openzone/data/glsl/submergedTerraWater.frag
    share/openzone/data/glsl/submergedTerraWater.vert
    share/openzone/data/glsl/terraLand.frag
    share/openzone/data/glsl/terraLand.vert
    share/openzone/data/glsl/terraWater.frag
    share/openzone/data/glsl/terraWater.vert
    share/openzone/data/glsl/ui.frag
    share/openzone/data/glsl/ui.vert
    DESTINATION share/openzone/data/glsl )

endif( OZ_INSTALL_DATA )
