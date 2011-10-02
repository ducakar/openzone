add_library( nirvana STATIC
  Device.cpp
  Lua.cpp
  Memo.cpp
  Mind.cpp
  Nirvana.cpp
  Task.cpp
  common.cpp
)
add_dependencies( nirvana matrix pch )
