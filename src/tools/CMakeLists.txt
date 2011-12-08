add_executable( checkBlanks checkBlanks.cc )
add_dependencies( checkBlanks pch )
target_link_libraries( checkBlanks oz )

add_executable( ozSineSample ozSineSample.cc )
add_dependencies( ozSineSample pch )
target_link_libraries( ozSineSample oz ${libs} )

add_executable( ozFormat ClassFormatter.cc ozFormat.cc )
add_dependencies( ozFormat pch )
target_link_libraries( ozFormat matrix common oz ${libs} )
