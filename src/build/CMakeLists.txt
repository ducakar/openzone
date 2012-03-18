add_executable( ozBuild BSP.cc Build.cc Caelum.cc Compiler.cc Context.cc Lingua.cc MD2.cc MD3.cc MeshData.cc OBJ.cc ozBuild.cc Terra.cc UI.cc )
add_dependencies( ozBuild pch )
target_link_libraries( ozBuild client modules nirvana matrix common build_info oz ${libs_tools} )

install( TARGETS ozBuild COMPONENT tools RUNTIME DESTINATION bin )
