add_library( arch STATIC
  Arch.cpp
  Arch.hpp
  common.cpp
  common.hpp
)

add_dependencies( arch nirvana matrix pch )
