add_library( modules STATIC
  common.hh
  Module.hh
  Modules.hh
  Module.cc
  Modules.cc
  flora/FloraModule.hh
  flora/FloraModule.cc )
use_pch( modules pch )
