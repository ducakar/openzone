if(NOT OZ_TOOLS)
  return()
endif()

add_library(builder STATIC
#BEGIN SOURCES
  AssImp.hh
  BSP.hh
  Builder.hh
  Caelum.hh
  Class.hh
  common.hh
  Compiler.hh
  Context.hh
  MD2.hh
  MD3.hh
  Terra.hh
  UI.hh
  AssImp.cc
  BSP.cc
  Builder.cc
  Caelum.cc
  Class.cc
  common.cc
  Compiler.cc
  Context.cc
  MD2.cc
  MD3.cc
  Terra.cc
  UI.cc
#END SOURCES
)
#target_precompile_headers(builder REUSE_FROM common)
