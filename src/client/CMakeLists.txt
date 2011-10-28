add_library( client STATIC
  BSP.cpp
  BasicAudio.cpp
  BotProxy.cpp
  BotAudio.cpp
  Caelum.cpp
  Camera.cpp
  Client.cpp
  Colours.cpp
  Compiler.cpp
  Context.cpp
  ExplosionImago.cpp
  Frustum.cpp
  GameStage.cpp
  Loader.cpp
  Lua.cpp
  MD2.cpp
  MD2Imago.cpp
  MD2WeaponImago.cpp
  MD3.cpp
  MD3Imago.cpp
  MenuStage.cpp
  Mesh.cpp
  Module.cpp
  Network.cpp
  OBJ.cpp
  OpenAL.cpp
  OpenGL.cpp
  Render.cpp
  SMM.cpp
  SMMImago.cpp
  SMMVehicleImago.cpp
  Shader.cpp
  Shape.cpp
  Audio.cpp
  Sound.cpp
  SparkModel.cpp
  Stage.cpp
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
  ui/InfoFrame.cpp
  ui/InventoryMenu.cpp
  ui/Keyboard.cpp
  ui/Label.cpp
  ui/LoadingArea.cpp
  ui/MainMenu.cpp
  ui/Mouse.cpp
  ui/SettingsFrame.cpp
  ui/StrategicArea.cpp
  ui/Text.cpp
  ui/UI.cpp
  modules/FloraModule.cpp
  modules/GalileoFrame.cpp
  modules/GalileoModule.cpp
  modules/MusicPlayer.cpp
  modules/MusicPlayerModule.cpp
  modules/ProfileModule.cpp
  modules/QuestFrame.cpp
  modules/QuestModule.cpp
)
add_dependencies( client pch )

add_executable( openzone openzone/openzone.cpp )
add_dependencies( openzone pch )
target_link_libraries( openzone build client nirvana matrix build oz ${libs} )

if( OZ_INSTALL_OPENZONE )
  install( TARGETS openzone DESTINATION bin )
endif()
