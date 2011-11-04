add_library( matrix STATIC
  BSP.cpp
  Bot.cpp
  BotClass.cpp
  Caelum.cpp
  Collider.cpp
  Dynamic.cpp
  DynamicClass.cpp
  Frag.cpp
  Library.cpp
  Lua.cpp
  Matrix.cpp
  NamePool.cpp
  Object.cpp
  ObjectClass.cpp
  Orbis.cpp
  Physics.cpp
  Struct.cpp
  Synapse.cpp
  Terra.cpp
  Vehicle.cpp
  VehicleClass.cpp
  Weapon.cpp
  WeaponClass.cpp
)
add_dependencies( matrix pch )
