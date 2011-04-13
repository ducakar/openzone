#!/bin/sh

echo 'add_executable( server' > CMakeLists.txt

ls *.{hpp,cpp} | sed -e 's/^./  \0/' >> CMakeLists.txt

echo ')' >> CMakeLists.txt
echo '' >> CMakeLists.txt
echo 'add_dependencies( server censor nirvana matrix pch )' >> CMakeLists.txt
echo 'target_link_libraries( server censor nirvana matrix oz )' >> CMakeLists.txt
