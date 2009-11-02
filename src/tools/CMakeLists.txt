add_executable( ozXmlrc ozXmlrc.cpp )
add_dependencies( ozXmlrc precompiled )
target_link_libraries( ozXmlrc base )
