add_executable( checkBlanks checkBlanks.cpp )
add_dependencies( checkBlanks pch )
target_link_libraries( checkBlanks oz )

add_executable( ozFormat ClassFormatter.cpp ozFormat.cpp )
add_dependencies( ozFormat pch )
target_link_libraries( ozFormat matrix common oz ${libs} )
