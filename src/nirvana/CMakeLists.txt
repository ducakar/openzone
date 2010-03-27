add_library( nirvana STATIC
common.cpp
common.h
Lua.cpp
Lua.h
LuaMind.cpp
LuaMind.h
Mind.cpp
Mind.h
Nirvana.cpp
Nirvana.h
RandomMind.cpp
RandomMind.h
Task.cpp
Task.h
)

add_dependencies( nirvana pch )
