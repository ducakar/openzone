add_library( builder STATIC
  BSP.hh
  Builder.hh
  Caelum.hh
  Class.hh
  common.hh
  Compiler.hh
  Context.hh
  FragPool.hh
  Lingua.hh
  MD2.hh
  MD3.hh
  OBJ.hh
  Terra.hh
  UI.hh
  BSP.cc
  Builder.cc
  Caelum.cc
  Class.cc
  common.cc
  Compiler.cc
  Context.cc
  FragPool.cc
  Lingua.cc
  MD2.cc
  MD3.cc
  OBJ.cc
  Terra.cc
  UI.cc )
use_pch( builder pch )
