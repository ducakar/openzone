add_library( modules STATIC
  common.hh
  Module.hh
  Modules.hh
  Module.cc
  Modules.cc
  flora/FloraModule.hh
  flora/FloraModule.cc )
add_dependencies( modules pch )
