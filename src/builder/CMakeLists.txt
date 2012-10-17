add_executable( ozBuild
  BSP.hh
  Builder.hh
  Caelum.hh
  Class.hh
  common.hh
  Compiler.hh
  Context.hh
  FragPool.hh
  Lingua.hh
  MD2.hh
  MD3.hh
  OBJ.hh
  Terra.hh
  UI.hh
  BSP.cc
  Builder.cc
  Caelum.cc
  Class.cc
  common.cc
  Compiler.cc
  Context.cc
  FragPool.cc
  Lingua.cc
  MD2.cc
  MD3.cc
  OBJ.cc
  ozBuild.cc
  Terra.cc
  UI.cc )
use_pch( ozBuild pch )
target_link_libraries( ozBuild client modules nirvana matrix common build_info oz ${libs_tools} )

if( OZ_STANDALONE )
  install( TARGETS ozBuild RUNTIME DESTINATION bin/${OZ_PLATFORM_NAME} )
else()
  install( TARGETS ozBuild RUNTIME DESTINATION bin )
endif()
