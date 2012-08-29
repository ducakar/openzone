add_library( common STATIC
  common.hh
  Lingua.hh
  luaapi.hh
  luabase.hh
  Lua.hh
  Timer.hh
  Lingua.cc
  luabase.cc
  Lua.cc
  Timer.cc )
add_dependencies( common pch )
