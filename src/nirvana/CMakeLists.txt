include_directories( .. )

add_library( nirvana STATIC
  M_Walker.cpp
  M_Walker.hpp
  Mind.cpp
  Mind.hpp
  Nirvana.cpp
  Nirvana.hpp
  Soul.cpp
  Soul.hpp )

add_dependencies( nirvana precompile )
