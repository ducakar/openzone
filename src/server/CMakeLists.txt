add_library( server STATIC
  Game.cpp
  Main.cpp
  Network.cpp
  Server.cpp
)

add_dependencies( server nirvana matrix pch )
