add_library( nirvana STATIC
#BEGIN SOURCES
  common.hh
  Device.hh
  luaapi.hh
  LuaNirvana.hh
  Memo.hh
  Mind.hh
  Nirvana.hh
  Task.hh
  Device.cc
  LuaNirvana.cc
  Memo.cc
  Mind.cc
  Nirvana.cc
  Task.cc
#END SOURCES
)

use_pch( nirvana pch )
