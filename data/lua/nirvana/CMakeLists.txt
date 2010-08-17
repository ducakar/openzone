add_custom_target( nirvana_droid ALL DEPENDS droid.luac )
add_custom_command( OUTPUT droid.luac
                    MAIN_DEPENDENCY droid.lua
                    COMMAND luac -o droid.luac droid.lua )
add_custom_target( nirvana_minds ALL DEPENDS minds.luac )
add_custom_command( OUTPUT minds.luac
                    MAIN_DEPENDENCY minds.lua
                    COMMAND luac -o minds.luac minds.lua )
