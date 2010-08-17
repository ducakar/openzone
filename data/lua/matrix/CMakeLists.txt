add_custom_target( matrix_handlers ALL DEPENDS handlers.luac )
add_custom_command( OUTPUT handlers.luac
                    MAIN_DEPENDENCY handlers.lua
                    COMMAND luac -o handlers.luac handlers.lua )
