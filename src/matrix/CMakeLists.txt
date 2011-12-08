add_library( matrix STATIC
  BSP.cc
  Bot.cc
  BotClass.cc
  Caelum.cc
  Collider.cc
  Dynamic.cc
  DynamicClass.cc
  Frag.cc
  FragPool.cc
  Library.cc
  Lua.cc
  Matrix.cc
  NamePool.cc
  Object.cc
  ObjectClass.cc
  Orbis.cc
  Physics.cc
  Struct.cc
  Synapse.cc
  Terra.cc
  Vehicle.cc
  VehicleClass.cc
  Weapon.cc
  WeaponClass.cc
)
add_dependencies( matrix pch )
