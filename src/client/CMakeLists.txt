add_executable( openzone
	Audio.cc
	BasicAudio.cc
	BotAudio.cc
	BotProxy.cc
	BSP.cc
	Camera.cc
	Colours.cc
	common.cc
	Context.cc
	ExplosionModel.cc
	FreeCamProxy.cc
	Frustum.cc
	GameStage.cc
	Main.cc
	MD2.cc
	MD2Model.cc
	MD2StaticModel.cc
	MD2WeaponModel.cc
	MD3.cc
	MD3StaticModel.cc
	Model.cc
	Network.cc
	OBJ.cc
	OBJModel.cc
	OBJVehicleModel.cc
	Render.cc
	Shape.cc
	Sky.cc
	Sound.cc
	SparkModel.cc
	StrategicProxy.cc
	Terrain.cc
	Water.cc
)

add_dependencies( openzone pch )
target_link_libraries( openzone ui nirvana matrix oz )
