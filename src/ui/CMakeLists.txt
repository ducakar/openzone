add_library( ui STATIC
Area.cpp
DebugArea.cpp
Font.cpp
HealthArea.cpp
ui.cpp
)

add_dependencies( ui precompile )
