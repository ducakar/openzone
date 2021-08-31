add_library(nirvana STATIC
#BEGIN SOURCES
  Device.cc
  Device.hh
  LuaNirvana.cc
  LuaNirvana.hh
  Memo.cc
  Memo.hh
  Mind.cc
  Mind.hh
  Nirvana.cc
  Nirvana.hh
  QuestList.cc
  QuestList.hh
  Task.cc
  Task.hh
  TechGraph.cc
  TechGraph.hh
  common.hh
  luaapi.cc
  luaapi.hh
#END SOURCES
)
target_precompile_headers(nirvana REUSE_FROM common)
