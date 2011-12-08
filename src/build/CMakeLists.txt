add_executable( ozBuild
  BSP.cc
  Caelum.cc
  Compiler.cc
  Context.cc
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
target_link_libraries( ozBuild client nirvana matrix common build oz ${libs_tools} )

if( OZ_INSTALL_TOOLS )
  install( TARGETS ozBuild DESTINATION bin )
endif()
