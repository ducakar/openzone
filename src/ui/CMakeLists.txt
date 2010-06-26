add_library( ui STATIC
	Area.cpp
	BuildMenu.cpp
	Button.cpp
	DebugFrame.cpp
	Font.cpp
	Frame.cpp
	HudArea.cpp
	InventoryMenu.cpp
	Keyboard.cpp
	Mouse.cpp
	StrategicArea.cpp
	UI.cpp
)

add_dependencies( ui pch )
