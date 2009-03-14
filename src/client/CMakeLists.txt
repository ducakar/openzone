link_directories( ../base ../matrix ../nirvana )
link_libraries( base matrix nirvana )

add_executable( openzone
  BSP.cpp
  BSP.hpp
  Camera.cpp
  Camera.hpp
  Client.cpp
  Client.hpp
  Context.hpp
  Context.cpp
  Font.cpp
  Font.hpp
  Frustum.cpp
  Frustum.hpp
  MD2.cpp
  MD2.hpp
  MD3.cpp
  MD3.hpp
  Main.cpp
  Main.hpp
  Model.hpp
  OBJ.cpp
  OBJ.hpp
  Particles.cpp
  Particles.hpp
  Render.cpp
  Render.hpp
  Shape.cpp
  Shape.hpp
  SimpleObject.cpp
  SimpleObject.hpp
  Sky.cpp
  Sky.hpp
  Skybox.cpp
  Skybox.hpp
  SoundManager.cpp
  SoundManager.hpp
  SparkGenRender.cpp
  SparkGenRender.hpp
  Terrain.cpp
  Terrain.hpp
  Water.cpp
  Water.hpp )

add_dependencies( openzone precompile )
