add_library( nirvana STATIC
common.cpp
common.hpp
Lua.cpp
Lua.hpp
LuaMind.cpp
LuaMind.hpp
Mind.cpp
Mind.hpp
Nirvana.cpp
Nirvana.hpp
RandomMind.cpp
RandomMind.hpp
Task.cpp
Task.hpp
)

add_dependencies( nirvana pch )
