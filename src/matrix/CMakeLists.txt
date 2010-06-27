add_library( matrix STATIC
	AABB.cc
	Bot.cc
	BotClass.cc
	BSP.cc
	Collider.cc
	Dynamic.cc
	DynamicClass.cc
	FloraManager.cc
	io.cc
	Lua.cc
	Matrix.cc
	Names.cc
	Object.cc
	ObjectClass.cc
	Particle.cc
	Physics.cc
	Sky.cc
	Structure.cc
	Synapse.cc
	Terrain.cc
	Timer.cc
	Translator.cc
	Vehicle.cc
	VehicleClass.cc
	Weapon.cc
	WeaponClass.cc
	World.cc
)

add_dependencies( matrix pch )
