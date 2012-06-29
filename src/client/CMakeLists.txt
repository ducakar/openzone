add_library( client STATIC Audio.cc BasicAudio.cc BotAudio.cc BSP.cc Caelum.cc Camera.cc CinematicProxy.cc Client.cc common.cc Context.cc ExplosionImago.cc FragPool.cc Frustum.cc GameStage.cc Imago.cc Loader.cc luaapi.cc Lua.cc MD2.cc MD2Imago.cc MD2WeaponImago.cc MD3.cc MD3Imago.cc MenuStage.cc Mesh.cc NaClDownloader.cc NaClGLContext.cc NaClMainCall.cc NaClUpdater.cc Network.cc OpenAL.cc OpenGL.cc  Profile.cc Proxy.cc QuestList.cc Render.cc Shader.cc Shape.cc SMM.cc SMMImago.cc SMMVehicleImago.cc Sound.cc SparkModel.cc Stage.cc StrategicProxy.cc Terra.cc UnitProxy.cc VehicleAudio.cc ui/Area.cc ui/BuildMenu.cc ui/Button.cc ui/DebugFrame.cc ui/Font.cc ui/Frame.cc ui/GalileoFrame.cc ui/HudArea.cc ui/InfoFrame.cc ui/InventoryMenu.cc ui/Keyboard.cc ui/Label.cc ui/LoadingArea.cc ui/MainMenu.cc ui/Mouse.cc ui/MusicPlayer.cc ui/QuestFrame.cc ui/SettingsFrame.cc ui/StrategicArea.cc ui/Text.cc ui/UI.cc )
add_dependencies( client pch )

add_executable( openzone openzone.cc )
add_dependencies( openzone pch )
target_link_libraries( openzone client modules nirvana matrix common build_info oz ${libs_client} )

if( NACL )
  add_custom_command( OUTPUT openzone.nmf openzone.html
    DEPENDS "${CMAKE_SOURCE_DIR}/etc/nacl/openzone.nmf" "${CMAKE_SOURCE_DIR}/etc/nacl/openzone.html"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/etc/nacl/openzone.nmf" openzone.nmf
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/etc/nacl/openzone.html" openzone.html )
  add_custom_target( openzone_files DEPENDS openzone.nmf openzone.html )

  add_dependencies( openzone openzone_files )
  set_target_properties( openzone PROPERTIES OUTPUT_NAME "openzone.${CMAKE_SYSTEM_PROCESSOR}.nexe" )
endif()

install( TARGETS openzone RUNTIME DESTINATION bin )
