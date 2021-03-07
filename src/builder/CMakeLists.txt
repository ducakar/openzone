if(NOT OZ_TOOLS)
  return()
endif()

add_library(builder STATIC
#BEGIN SOURCES
  AssImp.cc
  AssImp.hh
  BSP.cc
  BSP.hh
  Builder.cc
  Builder.hh
  Caelum.cc
  Caelum.hh
  Class.cc
  Class.hh
  Compiler.cc
  Compiler.hh
  Context.cc
  Context.hh
  MD2.cc
  MD2.hh
  MD3.cc
  MD3.hh
  Terra.cc
  Terra.hh
  UI.cc
  UI.hh
  common.cc
  common.hh
#END SOURCES
)
#target_precompile_headers(builder REUSE_FROM common)
