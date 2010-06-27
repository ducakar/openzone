add_executable( openzone
	Audio.cpp
	BasicAudio.cpp
	BotAudio.cpp
	BotProxy.cpp
	BSP.cpp
	Camera.cpp
	Colours.cpp
	common.cpp
	Context.cpp
	ExplosionModel.cpp
	FreeCamProxy.cpp
	Frustum.cpp
	GameStage.cpp
	Main.cpp
	MD2.cpp
	MD2Model.cpp
	MD2StaticModel.cpp
	MD2WeaponModel.cpp
	MD3.cpp
	MD3StaticModel.cpp
	Model.cpp
	Network.cpp
	OBJ.cpp
	OBJModel.cpp
	OBJVehicleModel.cpp
	Render.cpp
	Shape.cpp
	Sky.cpp
	Sound.cpp
	SparkModel.cpp
	StrategicProxy.cpp
	Terrain.cpp
	Water.cpp
)

add_dependencies( openzone pch )
target_link_libraries( openzone ui nirvana matrix oz )
