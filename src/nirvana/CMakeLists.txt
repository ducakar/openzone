add_library( nirvana STATIC
  Lua.cpp
  LuaMind.cpp
  Mind.cpp
  Nirvana.cpp
  RandomMind.cpp
  Task.cpp
  common.cpp
)

add_dependencies( nirvana matrix pch )
