add_library( censor STATIC
  Censor.cpp
  Censor.hpp
  common.cpp
  common.hpp
)

add_dependencies( censor nirvana matrix pch )
