add_library( common STATIC
#BEGIN SOURCES
  common.hh
  Lingua.hh
  luaapi.hh
  luabase.hh
  Lua.hh
  Timer.hh
  Lingua.cc
  luabase.cc
  Lua.cc
  Timer.cc
#END SOURCES
)
use_pch( common pch )
target_link_libraries( common ${LUA_LIBRARY} ozDynamics ozCore )

if( OZ_SDL2 )
  target_link_libraries( common ${SDL2_LIBRARY} )
endif()
