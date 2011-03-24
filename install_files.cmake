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
    mingw32/OpenAL32.dll
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
    data/class/beast.rc
    data/class/big crate.rc
    data/class/big explosion.rc
    data/class/bomb.rc
    data/class/cviček.rc
    data/class/drevo.rc
    data/class/droid commander.rc
    data/class/droid rifle.rc
    data/class/droid.rc
    data/class/first aid.rc
    data/class/goblin axe.rc
    data/class/goblin.rc
    data/class/knight.rc
    data/class/lord.rc
    data/class/metal barrel.rc
    data/class/palma.rc
    data/class/raptor.rc
    data/class/small crate.rc
    data/class/small explosion.rc
    data/class/smreka.rc
    data/class/tank.rc
    DESTINATION share/openzone/class )

  install( FILES
    data/fonts/DejaVuSans.ttf
    data/fonts/DejaVuSansMono.ttf
    DESTINATION share/openzone/fonts )

  install( FILES
    data/lua/matrix/handlers.luac
    DESTINATION share/openzone/lua/matrix )

  install( FILES
    data/lua/nirvana/droid.luac
    data/lua/nirvana/minds.luac
    DESTINATION share/openzone/lua/nirvana )

  install( FILES
    data/bsp/bunker.ozBSP
    data/bsp/bunker.ozcBSP
    data/bsp/castle.ozBSP
    data/bsp/castle.ozcBSP
    data/bsp/pool.ozBSP
    data/bsp/pool.ozcBSP
    DESTINATION share/openzone/bsp )

  install( FILES
    data/bsp/tex/_Drkalisce.ozcTex
    data/bsp/tex/_Potiskalnica.ozcTex
    data/bsp/tex/_Samostojeca_voda.ozcTex
    data/bsp/tex/_Spestalnica.ozcTex
    data/bsp/tex/crate1.ozcTex
    data/bsp/tex/glass.ozcTex
    data/bsp/tex/particle.ozcTex
    data/bsp/tex/roof1.ozcTex
    data/bsp/tex/slick.ozcTex
    data/bsp/tex/stone1.ozcTex
    data/bsp/tex/stone2.ozcTex
    data/bsp/tex/stone3.ozcTex
    data/bsp/tex/water1.ozcTex
    data/bsp/tex/wood1.ozcTex
    data/bsp/tex/wood2.ozcTex
    DESTINATION share/openzone/bsp/tex )

  install( FILES
    data/mdl/barrel1.ozcSMM
    data/mdl/bauul.ozcMD2
    data/mdl/bigCrate.ozcSMM
    data/mdl/bombs.ozcSMM
    data/mdl/droid-rifle.ozcMD2
    data/mdl/droid_commander.ozcMD2
    data/mdl/droid_infantry.ozcMD2
    data/mdl/explosion.ozcSMM
    data/mdl/goblin-axe.ozcMD2
    data/mdl/goblin.ozcMD2
    data/mdl/health.ozcSMM
    data/mdl/hobgoblin.ozcMD2
    data/mdl/knight.ozcMD2
    data/mdl/palmtree.ozcSMM
    data/mdl/raptor.ozcSMM
    data/mdl/smallCrate.ozcSMM
    data/mdl/tank.ozcSMM
    data/mdl/tree2.ozcSMM
    data/mdl/tree3.ozcSMM
    data/mdl/winebottle.ozcSMM
    DESTINATION share/openzone/mdl )

  install( FILES
    data/name/COPYING.wesnoth
    data/name/wesnoth.drake-male.txt
    DESTINATION share/openzone/name )

  install( FILES
    data/snd/cg2.wav
    data/snd/damage_bot1.wav
    data/snd/damage_bot2.wav
    data/snd/death1.wav
    data/snd/death2.wav
    data/snd/destroy_metal1.wav
    data/snd/destroy_wood1.wav
    data/snd/door.wav
    data/snd/explosion.wav
    data/snd/friction1.wav
    data/snd/gesture_flip.wav
    data/snd/hit1.wav
    data/snd/hit_bot1.wav
    data/snd/hit_bot2.wav
    data/snd/hit_metal1.wav
    data/snd/hit_metal2.wav
    data/snd/hit_wood1.wav
    data/snd/hit_wood2.wav
    data/snd/itemback.wav
    data/snd/jump1.wav
    data/snd/jump2.wav
    data/snd/land1.wav
    data/snd/land2.wav
    data/snd/outofammo.wav
    data/snd/splash1.wav
    data/snd/splash2.wav
    data/snd/tak.wav
    data/snd/tick.wav
    DESTINATION share/openzone/snd )

  install( FILES
    data/terra/heightmap.ozTerra
    data/terra/heightmap.ozcTerra
    DESTINATION share/openzone/terra )

  install( FILES
    data/sky/sky.ozcSky
    DESTINATION share/openzone/sky )

  install( FILES
    data/ui/COPYING
    data/ui/X_cursor.ozcCur
    data/ui/X_cursor.ozcTex
    data/ui/crosshair.ozcTex
    data/ui/fleur.ozcCur
    data/ui/fleur.ozcTex
    data/ui/grab.ozcTex
    data/ui/hand2.ozcCur
    data/ui/hand2.ozcTex
    data/ui/left_ptr.ozcCur
    data/ui/left_ptr.ozcTex
    data/ui/lift.ozcTex
    data/ui/mount.ozcTex
    data/ui/take.ozcTex
    data/ui/use.ozcTex
    data/ui/xterm.ozcCur
    data/ui/xterm.ozcTex
    DESTINATION share/openzone/ui )

  install( FILES
    data/glsl/bigMesh.frag
    data/glsl/bigMesh.vert
    data/glsl/celestial.frag
    data/glsl/celestial.vert
    data/glsl/header.glsl
    data/glsl/md2.frag
    data/glsl/md2.vert
    data/glsl/mesh.frag
    data/glsl/mesh.vert
    data/glsl/particles.frag
    data/glsl/particles.vert
    data/glsl/plant.frag
    data/glsl/plant.vert
    data/glsl/simple.frag
    data/glsl/simple.vert
    data/glsl/stars.frag
    data/glsl/stars.vert
    data/glsl/terraLand.frag
    data/glsl/terraLand.vert
    data/glsl/terraWater.frag
    data/glsl/terraWater.vert
    data/glsl/ui.frag
    data/glsl/ui.vert
    DESTINATION share/openzone/glsl )

endif( OZ_INSTALL_DATA )
