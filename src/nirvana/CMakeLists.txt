add_library( nirvana STATIC common.cc Device.cc Lua.cc Memo.cc Mind.cc Nirvana.cc Task.cc )
add_dependencies( nirvana pch )
