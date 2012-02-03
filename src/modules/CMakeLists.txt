add_library( modules STATIC
  Modules.cc
  flora/FloraModule.cc
)
add_dependencies( modules pch )
