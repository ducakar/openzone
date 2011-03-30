add_custom_target( lua_nirvana_droid ALL
                   SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/droid.lua
                   DEPENDS droid.luac )
add_custom_command( OUTPUT droid.luac
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/droid.lua
                    COMMAND luac -o droid.luac ${CMAKE_CURRENT_SOURCE_DIR}/droid.lua )
add_custom_target( lua_nirvana_minds ALL
                   SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/minds.lua
                   DEPENDS minds.luac )
add_custom_command( OUTPUT minds.luac
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/minds.lua
                    COMMAND luac -o minds.luac ${CMAKE_CURRENT_SOURCE_DIR}/minds.lua )
