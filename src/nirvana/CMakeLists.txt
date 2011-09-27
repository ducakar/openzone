add_library( nirvana STATIC
  Lua.cpp
  Mind.cpp
  Nirvana.cpp
  Scientia.cpp
  Task.cpp
  common.cpp
)
add_dependencies( nirvana matrix pch )
