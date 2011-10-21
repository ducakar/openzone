add_library( server STATIC
  Game.cpp
  Main.cpp
  Network.cpp
)
add_dependencies( server pch )
