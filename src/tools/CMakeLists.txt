add_executable( openzone openzone.cpp )
add_dependencies( openzone client ui nirvana matrix pch )
target_link_libraries( openzone client ui nirvana matrix oz )

add_executable( ozXmlrc ozXmlrc.cpp )
add_dependencies( ozXmlrc pch )
target_link_libraries( ozXmlrc oz )

add_executable( ozPrebuildData ozPrebuildData.cpp )
add_dependencies( ozPrebuildData matrix pch )
target_link_libraries( ozPrebuildData client ui nirvana matrix oz )
