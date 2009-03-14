link_directories( ../base ../matrix ../nirvana )
link_libraries( base matrix nirvana )

add_executable( server
  Main.cpp
  Main.hpp
  Server.cpp
  Server.hpp )

add_dependencies( server precompile )
