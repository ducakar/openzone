add_executable( server
	Game.cpp
	Main.cpp
	Network.cpp
	Server.cpp
)

add_dependencies( server pch )
target_link_libraries( server nirvana matrix base )
