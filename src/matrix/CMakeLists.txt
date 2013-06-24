add_library( matrix STATIC
#BEGIN SOURCES
  BotClass.hh
  Bot.hh
  BSP.hh
  Caelum.hh
  Collider.hh
  common.hh
  DynamicClass.hh
  Dynamic.hh
  Frag.hh
  FragPool.hh
  Liber.hh
  luaapi.hh
  Lua.hh
  Matrix.hh
  NamePool.hh
  ObjectClass.hh
  Object.hh
  Orbis.hh
  Physics.hh
  Struct.hh
  Synapse.hh
  Terra.hh
  VehicleClass.hh
  Vehicle.hh
  WeaponClass.hh
  Weapon.hh
  Bot.cc
  BotClass.cc
  BSP.cc
  Caelum.cc
  Collider.cc
  Dynamic.cc
  DynamicClass.cc
  Frag.cc
  FragPool.cc
  Liber.cc
  luaapi.cc
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
#END SOURCES
)

use_pch( matrix pch )
