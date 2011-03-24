add_library( architect STATIC
  Architect.cpp
  Architect.hpp
  common.cpp
  common.hpp
)

add_dependencies( architect nirvana matrix pch )
