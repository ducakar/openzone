add_executable( checkBlanks checkBlanks.cpp )
add_dependencies( checkBlanks pch )
target_link_libraries( checkBlanks oz )
