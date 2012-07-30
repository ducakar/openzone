add_library( client STATIC
  Audio.cc
  BasicAudio.cc
  BotAudio.cc
  BSPAudio.cc
  BSP.cc
  Caelum.cc
  Camera.cc
  CinematicProxy.cc
  Client.cc
  common.cc
  Context.cc
  ExplosionImago.cc
  FragPool.cc
  Frustum.cc
  GameStage.cc
  Imago.cc
  Input.cc
  Loader.cc
  luaapi.cc
  Lua.cc
  MD2.cc
  MD2Imago.cc
  MD2WeaponImago.cc
  MD3.cc
  MD3Imago.cc
  MenuStage.cc
  Mesh.cc
  NaCl.cc
  NaClDownloader.cc
  NaClUpdater.cc
  Network.cc
  OpenAL.cc
  OpenGL.cc
  Profile.cc
  Proxy.cc
  QuestList.cc
  Render.cc
  Shader.cc
  Shape.cc
  SMM.cc
  SMMImago.cc
  SMMVehicleImago.cc
  Sound.cc
  SparkModel.cc
  Stage.cc
  StrategicProxy.cc
  Terra.cc
  UnitProxy.cc
  VehicleAudio.cc
  Window.cc
  ui/Area.cc
  ui/BuildMenu.cc
  ui/Button.cc
  ui/CinematicText.cc
  ui/CreditsMenu.cc
  ui/DebugFrame.cc
  ui/Font.cc
  ui/Frame.cc
  ui/GalileoFrame.cc
  ui/HudArea.cc
  ui/InfoFrame.cc
  ui/Inventory.cc
  ui/Label.cc
  ui/LoadingArea.cc
  ui/MainMenu.cc
  ui/MissionMenu.cc
  ui/Mouse.cc
  ui/MusicPlayer.cc
  ui/QuestFrame.cc
  ui/Scheme.cc
  ui/SettingsFrame.cc
  ui/StrategicArea.cc
  ui/Text.cc
  ui/UI.cc )
add_dependencies( client pch )

add_executable( openzone openzone.cc )
add_dependencies( openzone pch )
target_link_libraries( openzone client modules nirvana matrix common build_info oz ${libs_client} )

if( NACL )
  set_target_properties( openzone PROPERTIES OUTPUT_NAME "openzone.${CMAKE_SYSTEM_PROCESSOR}.nexe" )
endif()

if( OZ_STANDALONE )
  install( TARGETS openzone RUNTIME DESTINATION bin/${OZ_SYSTEM_NAME} )
else()
  install( TARGETS openzone RUNTIME DESTINATION bin )
endif()
