configure_file( config.hh.in config.hh @ONLY )
configure_file( BuildInfo.cc.in BuildInfo.cc @ONLY )

add_library( client STATIC
  ${CMAKE_CURRENT_BINARY_DIR}/BuildInfo.cc
#BEGIN SOURCES
  Audio.hh
  BasicAudio.hh
  BotAudio.hh
  BSPAudio.hh
  BSP.hh
  BuildInfo.hh
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
  ui/UI.cc
#END SOURCES
)
use_pch( client pch )

if( OZ_SDL2 )
  target_link_libraries( client ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} )
else()
  target_link_libraries( client ${SDL_LIBRARY} ${SDL_TTF_LIBRARY} )
endif()

if( OZ_GL_ES )
  if( ANDROID )
    target_link_libraries( client -lGLESv2 )
  elseif( NACL )
    target_link_libraries( client -lppapi_gles2 )
  else()
    target_link_libraries( client ${GLES2_LIBRARIES} )
  endif()
else()
  target_link_libraries( client ${OPENGL_gl_LIBRARY} )
endif()

target_link_libraries( client ${OPENAL_LIBRARY} ${VORBISFILE_LIBRARIES} )

if( PLATFORM_EMBEDDED )
  target_link_libraries( client ${FREETYPE_LIBRARY} ${VORBIS_LIBRARIES} ${OGG_LIBRARIES} )
endif()
