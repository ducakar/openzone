add_library(common STATIC
#BEGIN SOURCES
  AABB.hh
  Bounds.hh
  Lingua.cc
  Lingua.hh
  Timer.cc
  Timer.hh
  common.hh
  luaapi.hh
  luabase.cc
  luabase.hh
#END SOURCES
)
#target_precompile_headers(common PUBLIC common.hh)
