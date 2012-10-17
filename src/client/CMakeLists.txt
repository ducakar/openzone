add_library( client STATIC
  Audio.hh
  BasicAudio.hh
  BotAudio.hh
  BSPAudio.hh
  BSP.hh
  Caelum.hh
  Camera.hh
  CinematicProxy.hh
  Client.hh
  common.hh
  Context.hh
  eSpeak.hh
  ExplosionImago.hh
  FragPool.hh
  Frustum.hh
  GameStage.hh
  Imago.hh
  Input.hh
  Loader.hh
  luaapi.hh
  Lua.hh
  MD2.hh
  MD2Imago.hh
  MD2WeaponImago.hh
  MD3.hh
  MD3Imago.hh
  MenuStage.hh
  Mesh.hh
  NaClDownloader.hh
  NaCl.hh
  NaClUpdater.hh
  Network.hh
  OpenAL.hh
  OpenGL.hh
  openzone.hh
  Profile.hh
  Proxy.hh
  QuestList.hh
  Render.hh
  Shader.hh
  Shape.hh
  SMM.hh
  SMMImago.hh
  SMMVehicleImago.hh
  Sound.hh
  SparkModel.hh
  Stage.hh
  StrategicProxy.hh
  Terra.hh
  UnitProxy.hh
  VehicleAudio.hh
  Window.hh
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
  eSpeak.cc
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
  ui/Area.hh
  ui/BuildMenu.hh
  ui/Button.hh
  ui/CinematicText.hh
  ui/CreditsMenu.hh
  ui/DebugFrame.hh
  ui/Font.hh
  ui/Frame.hh
  ui/GalileoFrame.hh
  ui/HudArea.hh
  ui/InfoFrame.hh
  ui/Inventory.hh
  ui/Label.hh
  ui/LoadingArea.hh
  ui/MainMenu.hh
  ui/MissionMenu.hh
  ui/Mouse.hh
  ui/MusicPlayer.hh
  ui/QuestFrame.hh
  ui/SelectionArea.hh
  ui/SettingsFrame.hh
  ui/Style.hh
  ui/Text.hh
  ui/UI.hh
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
  ui/SelectionArea.cc
  ui/SettingsFrame.cc
  ui/Style.cc
  ui/Text.cc
  ui/UI.cc )
use_pch( client pch )

if( ANDROID )
  set( jniLibPath "${CMAKE_BINARY_DIR}/../Android/libs/${PLATFORM_PROC_PREFIX}/libopenzone.so" )

  add_library( openzone SHARED openzone.cc )

  add_custom_command( OUTPUT "${jniLibPath}" DEPENDS openzone
                      COMMAND "${CMAKE_COMMAND}" -E copy "libopenzone.so" "${jniLibPath}" )
  add_custom_target( openzone_android ALL DEPENDS android_project "${jniLibPath}" )
else()
  add_executable( openzone openzone.cc )
endif()
use_pch( openzone pch )
target_link_libraries( openzone client modules nirvana matrix common build_info oz ${libs_client} )

if( NACL )
  set_target_properties( openzone PROPERTIES OUTPUT_NAME "openzone.${CMAKE_SYSTEM_PROCESSOR}.nexe" )
endif()

if( OZ_STANDALONE )
  install( TARGETS openzone RUNTIME DESTINATION bin/${OZ_PLATFORM_NAME} LIBRARY )
else()
  install( TARGETS openzone RUNTIME DESTINATION bin LIBRARY DESTINATION lib )
endif()
