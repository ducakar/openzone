add_definitions( -DOZ_TOOLS )

add_library( matrix-tools STATIC
  ../matrix/AABB.cpp
  ../matrix/BSP.cpp
  ../matrix/Bot.cpp
  ../matrix/BotClass.cpp
  ../matrix/Caelum.cpp
  ../matrix/Collider.cpp
  ../matrix/Dynamic.cpp
  ../matrix/DynamicClass.cpp
  ../matrix/Lua.cpp
  ../matrix/Matrix.cpp
  ../matrix/NamePool.cpp
  ../matrix/Object.cpp
  ../matrix/ObjectClass.cpp
  ../matrix/Orbis.cpp
  ../matrix/Particle.cpp
  ../matrix/Physics.cpp
  ../matrix/QBSP.cpp
  ../matrix/Struct.cpp
  ../matrix/Synapse.cpp
  ../matrix/Terra.cpp
  ../matrix/Timer.cpp
  ../matrix/Translator.cpp
  ../matrix/Vehicle.cpp
  ../matrix/VehicleClass.cpp
  ../matrix/Weapon.cpp
  ../matrix/WeaponClass.cpp
  ../matrix/common.cpp
  ../matrix/modules/FloraModule.cpp
)
add_dependencies( matrix-tools pch )

add_library( client-tools STATIC
  ../client/Audio.cpp
  ../client/BSP.cpp
  ../client/BasicAudio.cpp
  ../client/BotAudio.cpp
  ../client/BotProxy.cpp
  ../client/Caelum.cpp
  ../client/Camera.cpp
  ../client/Client.cpp
  ../client/Colours.cpp
  ../client/Compiler.cpp
  ../client/Context.cpp
  ../client/ExplosionModel.cpp
  ../client/Frustum.cpp
  ../client/GameStage.cpp
  ../client/Loader.cpp
  ../client/MD2.cpp
  ../client/MD2Model.cpp
  ../client/MD2WeaponModel.cpp
  ../client/MD3.cpp
  ../client/Mesh.cpp
  ../client/Network.cpp
  ../client/OBJ.cpp
  ../client/OpenAL.cpp
  ../client/OpenGL.cpp
  ../client/Render.cpp
  ../client/SMM.cpp
  ../client/SMMModel.cpp
  ../client/SMMVehicleModel.cpp
  ../client/Shader.cpp
  ../client/Shape.cpp
  ../client/Sound.cpp
  ../client/SparkModel.cpp
  ../client/StrategicProxy.cpp
  ../client/Terra.cpp
  ../client/VehicleAudio.cpp
  ../client/common.cpp
  ../client/ui/Area.cpp
  ../client/ui/BuildMenu.cpp
  ../client/ui/Button.cpp
  ../client/ui/DebugFrame.cpp
  ../client/ui/Font.cpp
  ../client/ui/Frame.cpp
  ../client/ui/HudArea.cpp
  ../client/ui/InventoryMenu.cpp
  ../client/ui/Keyboard.cpp
  ../client/ui/LoadingArea.cpp
  ../client/ui/Mouse.cpp
  ../client/ui/MusicPlayer.cpp
  ../client/ui/StrategicArea.cpp
  ../client/ui/UI.cpp
)
add_dependencies( client-tools nirvana matrix-tools pch )

add_executable( ozPrebuild ozPrebuild.cpp )
add_dependencies( ozPrebuild client-tools nirvana matrix-tools build pch )
target_link_libraries( ozPrebuild client-tools nirvana matrix-tools build oz ${libs_tools} )

if( OZ_INSTALL_TOOLS )
  install( TARGETS ozPrebuild DESTINATION bin COMPONENT tools )
endif()
