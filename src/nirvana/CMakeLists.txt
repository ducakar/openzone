add_library( nirvana STATIC
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
	common.cpp
	common.hpp
)

add_dependencies( nirvana pch )
