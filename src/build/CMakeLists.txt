add_definitions( -DOZ_TOOLS )

add_library( client-tools STATIC
  ../client/Caelum.cpp
  ../client/Compiler.cpp
  ../client/Context.cpp
  ../client/MD2.cpp
  ../client/MD3.cpp
  ../client/Mesh.cpp
  ../client/Module.cpp
  ../client/OBJ.cpp
  ../client/OpenGL.cpp
  ../client/Render.cpp
  ../client/Shape.cpp
  ../client/common.cpp
  ../client/ui/Mouse.cpp
  ../client/modules/GalileoModule.cpp
)
add_dependencies( client-tools pch )

add_executable( ozPrebuild
  BSP.cpp
  Terra.cpp
  ozPrebuild.cpp
)
add_dependencies( ozPrebuild pch )
target_link_libraries( ozPrebuild client-tools nirvana matrix common build oz ${libs_tools} )

if( OZ_INSTALL_TOOLS )
  install( TARGETS ozPrebuild DESTINATION bin )
endif()
