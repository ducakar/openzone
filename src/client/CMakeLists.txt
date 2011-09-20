add_library( client STATIC
  Audio.cpp
  BSP.cpp
  BasicAudio.cpp
  BotAudio.cpp
  BotProxy.cpp
  Caelum.cpp
  Camera.cpp
  Client.cpp
  Colours.cpp
  Compiler.cpp
  Context.cpp
  ExplosionModel.cpp
  Frustum.cpp
  GameStage.cpp
  Loader.cpp
  MD2.cpp
  MD2Model.cpp
  MD2WeaponModel.cpp
  MD3.cpp
  Mesh.cpp
  Network.cpp
  OBJ.cpp
  OpenAL.cpp
  OpenGL.cpp
  Render.cpp
  SMM.cpp
  SMMModel.cpp
  SMMVehicleModel.cpp
  Shader.cpp
  Shape.cpp
  Sound.cpp
  SparkModel.cpp
  StrategicProxy.cpp
  Terra.cpp
  VehicleAudio.cpp
  common.cpp
  ui/Area.cpp
  ui/BuildMenu.cpp
  ui/Button.cpp
  ui/DebugFrame.cpp
  ui/Font.cpp
  ui/Frame.cpp
  ui/HudArea.cpp
  ui/InventoryMenu.cpp
  ui/Keyboard.cpp
  ui/LoadingArea.cpp
  ui/Mouse.cpp
  ui/MusicPlayer.cpp
  ui/StrategicArea.cpp
  ui/UI.cpp
)
add_dependencies( client nirvana matrix pch )

add_executable( openzone openzone/openzone.cpp )
add_dependencies( openzone client nirvana matrix build pch )
target_link_libraries( openzone build client nirvana matrix build oz ${libs} )

if( OZ_INSTALL_OPENZONE )
  install( TARGETS openzone DESTINATION bin COMPONENT openzone )
endif()
