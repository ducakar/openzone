add_library( nirvana STATIC
  Lua.cpp
  Meme.cpp
  Mind.cpp
  Nirvana.cpp
  Scientia.cpp
  Task.cpp
  common.cpp
)
add_dependencies( nirvana matrix pch )
