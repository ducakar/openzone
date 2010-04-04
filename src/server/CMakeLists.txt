add_executable( server
Game.cpp
Game.h
Main.cpp
Main.h
Network.cpp
Network.h
Server.cpp
Server.h
)

add_dependencies( server pch )
target_link_libraries( server nirvana matrix base )
