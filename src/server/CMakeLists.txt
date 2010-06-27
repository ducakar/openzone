add_executable( server
	Game.cc
	Main.cc
	Network.cc
	Server.cc
)

add_dependencies( server pch )
target_link_libraries( server nirvana matrix base )
