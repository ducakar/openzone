add_executable( ozBuild
  BSP.cpp
  Caelum.cpp
  Compiler.cpp
  Context.cpp
  MD2.cpp
  MD3.cpp
  Mesh.cpp
  Mouse.cpp
  OBJ.cpp
  Terra.cpp
  ozBuild.cpp
  modules/GalileoModule.cpp
)
add_dependencies( ozBuild pch )
target_link_libraries( ozBuild client nirvana matrix common build oz ${libs_tools} )

if( OZ_INSTALL_TOOLS )
  install( TARGETS ozBuild DESTINATION bin )
endif()
