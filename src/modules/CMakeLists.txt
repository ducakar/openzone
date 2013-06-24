add_library( modules STATIC
#BEGIN SOURCES
  common.hh
  Module.hh
  Modules.hh
  Module.cc
  Modules.cc
  flora/FloraModule.hh
  flora/FloraModule.cc
#END SOURCES
)

use_pch( modules pch )
