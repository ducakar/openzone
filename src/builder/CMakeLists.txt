if( PLATFORM_EMBEDDED )
  return()
endif()

configure_file( config.hh.in config.hh @ONLY )

add_library( builder STATIC
#BEGIN SOURCES
  AssImp.hh
  BSP.hh
  Builder.hh
  Caelum.hh
  Class.hh
  common.hh
  Compiler.hh
  Context.hh
  MD2.hh
  MD3.hh
  Terra.hh
  UI.hh
  AssImp.cc
  BSP.cc
  Builder.cc
  Caelum.cc
  Class.cc
  common.cc
  Compiler.cc
  Context.cc
  MD2.cc
  MD3.cc
  Terra.cc
  UI.cc
#END SOURCES
)

use_pch( builder pch )

if( OZ_NONFREE )
  target_link_libraries( builder ${SQUISH_LIBRARY} )
endif()

target_link_libraries( builder ${FREEIMAGE_LIBRARY} ${ASSIMP_LIBRARIES} )
