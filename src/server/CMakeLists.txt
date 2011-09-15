add_library( server STATIC
  Game.cpp
  Game.hpp
  Main.cpp
  Main.hpp
  Network.cpp
  Network.hpp
  Server.cpp
  Server.hpp
)

add_dependencies( server arch nirvana matrix pch )
