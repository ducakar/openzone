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
use_pch( common pch )
target_link_libraries( common ${LUA_LIBRARY} ${SDL_LIBRARY} oz )
