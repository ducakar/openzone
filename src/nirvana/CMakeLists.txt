add_library( nirvana STATIC
	common.cpp
	Lua.cpp
	LuaMind.cpp
	Mind.cpp
	Nirvana.cpp
	RandomMind.cpp
	Task.cpp
)

add_dependencies( nirvana pch )
