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

add_dependencies( server censor nirvana matrix pch )
target_link_libraries( server censor nirvana matrix oz )
