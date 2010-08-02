add_executable( server
	Game.cpp
	Game.hpp
	Main.cpp
	Main.hpp
	Network.cpp
	Network.hpp
	Server.cpp
	Server.hpp
)

add_dependencies( server pch )
target_link_libraries( server nirvana matrix oz ${libs} )
