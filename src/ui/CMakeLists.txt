add_library( ui STATIC
Area.cpp
Area.h
BuildMenu.cpp
BuildMenu.h
Button.cpp
Button.h
DebugFrame.cpp
DebugFrame.h
Font.cpp
Font.h
Frame.cpp
Frame.h
HudArea.cpp
HudArea.h
InventoryMenu.cpp
InventoryMenu.h
Keyboard.cpp
Keyboard.h
Mouse.cpp
Mouse.h
StrategicArea.cpp
StrategicArea.h
UI.cpp
UI.h
)

add_dependencies( ui pch )
