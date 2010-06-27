add_library( nirvana STATIC
	common.cc
	Lua.cc
	LuaMind.cc
	Mind.cc
	Nirvana.cc
	RandomMind.cc
	Task.cc
)

add_dependencies( nirvana pch )
