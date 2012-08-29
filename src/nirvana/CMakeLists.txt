add_library( nirvana STATIC
  common.hh
  Device.hh
  luaapi.hh
  Lua.hh
  Memo.hh
  Mind.hh
  Nirvana.hh
  Task.hh
  common.cc
  Device.cc
  Lua.cc
  Memo.cc
  Mind.cc
  Nirvana.cc
  Task.cc )
add_dependencies( nirvana pch )
