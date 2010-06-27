add_library( ui STATIC
	Area.cc
	BuildMenu.cc
	Button.cc
	DebugFrame.cc
	Font.cc
	Frame.cc
	HudArea.cc
	InventoryMenu.cc
	Keyboard.cc
	Mouse.cc
	StrategicArea.cc
	UI.cc
)

add_dependencies( ui pch )
