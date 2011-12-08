add_library( nirvana STATIC
  Device.cc
  Lua.cc
  Memo.cc
  Mind.cc
  Nirvana.cc
  Task.cc
  common.cc
)
add_dependencies( nirvana pch )
