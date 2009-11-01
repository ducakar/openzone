add_executable( ozXmlrc ozXmlrc.cpp )
add_dependencies( ozXmlrc precompile )
target_link_libraries( ozXmlrc base )
