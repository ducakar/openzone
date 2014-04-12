add_library( nirvana STATIC
#BEGIN SOURCES
  Automaton.hh
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
  Automaton.cc
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

use_pch( nirvana pch )
