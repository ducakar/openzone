add_executable( ozBuild
  BSP.cc
  Builder.cc
  Caelum.cc
  Class.cc
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
add_dependencies( ozBuild pch )
target_link_libraries( ozBuild client modules nirvana matrix common build_info oz ${libs_tools} )

if( OZ_STANDALONE )
  install( TARGETS ozBuild RUNTIME DESTINATION bin/${OZ_SYSTEM_NAME} )
else()
  install( TARGETS ozBuild RUNTIME DESTINATION bin )
endif()
