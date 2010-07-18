add_library( matrix STATIC
	AABB.cpp
	BotClass.cpp
	Bot.cpp
	BSP.cpp
	Collider.cpp
	DynamicClass.cpp
	Dynamic.cpp
	FloraManager.cpp
	io.cpp
	Lua.cpp
	Matrix.cpp
	Names.cpp
	ObjectClass.cpp
	Object.cpp
	Orbis.cpp
	Particle.cpp
	Physics.cpp
	Sky.cpp
	Structure.cpp
	Synapse.cpp
	Terra.cpp
	Timer.cpp
	Translator.cpp
	VehicleClass.cpp
	Vehicle.cpp
	WeaponClass.cpp
	Weapon.cpp
)

add_dependencies( matrix pch )
