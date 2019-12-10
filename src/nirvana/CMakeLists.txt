add_library(nirvana STATIC
#BEGIN SOURCES
  common.hh
  Device.hh
  luaapi.hh
  LuaNirvana.hh
  Memo.hh
  Mind.hh
  Nirvana.hh
  QuestList.hh
  Task.hh
  TechGraph.hh
  Device.cc
  luaapi.cc
  LuaNirvana.cc
  Memo.cc
  Mind.cc
  Nirvana.cc
  QuestList.cc
  Task.cc
  TechGraph.cc
#END SOURCES
)
target_precompile_headers(nirvana REUSE_FROM common)
