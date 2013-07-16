add_library( common STATIC
#BEGIN SOURCES
  common.hh
  Lingua.hh
  luaapi.hh
  luabase.hh
  LuaCommon.hh
  Timer.hh
  Lingua.cc
  luabase.cc
  LuaCommon.cc
  Timer.cc
#END SOURCES
)

use_pch( common pch )

if( OZ_LUAJIT )
  target_link_libraries( common ${LUAJIT_LIBRARIES} )
else()
  target_link_libraries( common ${LUA_LIBRARIES} )
endif()
