add_executable( ozBuild
  BSP.cc
  Build.cc
  Caelum.cc
  Compiler.cc
  Context.cc
  Lingua.cc
  MD2.cc
  MD3.cc
  Mesh.cc
  Mouse.cc
  OBJ.cc
  Terra.cc
  ozBuild.cc
  modules/GalileoModule.cc
)
add_dependencies( ozBuild pch )
target_link_libraries( ozBuild client nirvana matrix common build_info oz ${libs_tools} )

if( OZINST_TOOLS )
  install( TARGETS ozBuild DESTINATION bin )
endif()
