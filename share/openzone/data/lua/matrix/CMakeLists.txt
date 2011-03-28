add_custom_target( lua_matrix_handlers ALL
                   SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/handlers.lua
                   DEPENDS handlers.luac )
add_custom_command( OUTPUT handlers.luac
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/handlers.lua
                    COMMAND luac -o handlers.luac ${CMAKE_CURRENT_SOURCE_DIR}/handlers.lua )
