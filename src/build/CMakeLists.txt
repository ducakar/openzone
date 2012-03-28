add_executable( ozbuild BSP.cc Build.cc Caelum.cc Compiler.cc Context.cc Lingua.cc MD2.cc MD3.cc MeshData.cc OBJ.cc ozbuild.cc Terra.cc UI.cc )
add_dependencies( ozbuild pch )
target_link_libraries( ozbuild client modules nirvana matrix common build_info oz ${libs_tools} )

install( TARGETS ozbuild COMPONENT tools RUNTIME DESTINATION bin )
