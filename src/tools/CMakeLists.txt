add_definitions( -DOZ_TOOLS )

add_library( matrix-tools STATIC
  ../matrix/AABB.cpp
  ../matrix/Caelum.cpp
  ../matrix/common.cpp
  ../matrix/Orbis.cpp
  ../matrix/QBSP.cpp
  ../matrix/Timer.cpp
  ../matrix/Terra.cpp
  ../matrix/Library.cpp
)
add_dependencies( matrix-tools pch )

add_library( client-tools STATIC
  ../client/BSP.cpp
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
  ../client/Terra.cpp
  ../client/common.cpp
  ../client/ui/Mouse.cpp
  ../client/modules/GalileoModule.cpp
)
add_dependencies( client-tools pch )

add_executable( ozPrebuild ozPrebuild.cpp )
add_dependencies( ozPrebuild pch )
target_link_libraries( ozPrebuild client-tools matrix-tools build oz ${libs_tools} )

if( OZ_INSTALL_TOOLS )
  install( TARGETS ozPrebuild DESTINATION bin )
endif()
