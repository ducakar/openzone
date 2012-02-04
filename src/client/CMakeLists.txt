add_library( client STATIC
  Audio.cc
  BSP.cc
  BasicAudio.cc
  BotAudio.cc
  BotProxy.cc
  Caelum.cc
  Camera.cc
  Client.cc
  Colours.cc
  Context.cc
  ExplosionImago.cc
  FragPool.cc
  Frustum.cc
  GameStage.cc
  Loader.cc
  Lua.cc
  MD2.cc
  MD2Imago.cc
  MD2WeaponImago.cc
  MD3.cc
  MD3Imago.cc
  MenuStage.cc
  Mesh.cc
  Network.cc
  OpenAL.cc
  OpenGL.cc
  Profile.cc
  QuestList.cc
  Render.cc
  SMM.cc
  SMMImago.cc
  SMMVehicleImago.cc
  Shader.cc
  Shape.cc
  Sound.cc
  SparkModel.cc
  Stage.cc
  StrategicProxy.cc
  Terra.cc
  VehicleAudio.cc
  common.cc
  ui/Area.cc
  ui/BuildMenu.cc
  ui/Button.cc
  ui/DebugFrame.cc
  ui/Font.cc
  ui/Frame.cc
  ui/GalileoFrame.cc
  ui/HudArea.cc
  ui/InfoFrame.cc
  ui/InventoryMenu.cc
  ui/Keyboard.cc
  ui/Label.cc
  ui/LoadingArea.cc
  ui/MainMenu.cc
  ui/Mouse.cc
  ui/MusicPlayer.cc
  ui/QuestFrame.cc
  ui/SettingsFrame.cc
  ui/StrategicArea.cc
  ui/Text.cc
  ui/UI.cc
)
add_dependencies( client pch )

add_executable( openzone openzone/openzone.cc )
add_dependencies( openzone pch )
target_link_libraries( openzone client modules nirvana matrix common build_info oz ${libs_client} )

if( OZ_INSTALL_CLIENT )
  install( TARGETS openzone DESTINATION bin )
endif()
