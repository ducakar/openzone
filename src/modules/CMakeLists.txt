add_library( modules STATIC Module.cc Modules.cc flora/FloraModule.cc )
add_dependencies( modules pch )
