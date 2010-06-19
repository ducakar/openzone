add_library( ui STATIC
Area.cpp
Area.hpp
BuildMenu.cpp
BuildMenu.hpp
Button.cpp
Button.hpp
DebugFrame.cpp
DebugFrame.hpp
Font.cpp
Font.hpp
Frame.cpp
Frame.hpp
HudArea.cpp
HudArea.hpp
InventoryMenu.cpp
InventoryMenu.hpp
Keyboard.cpp
Keyboard.hpp
Mouse.cpp
Mouse.hpp
StrategicArea.cpp
StrategicArea.hpp
UI.cpp
UI.hpp
)

add_dependencies( ui pch )
