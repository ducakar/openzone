#!/bin/sh

echo 'add_library( arch STATIC' > CMakeLists.txt

ls *.{hpp,cpp} | sed -e 's/^./  \0/' >> CMakeLists.txt

echo ')' >> CMakeLists.txt
echo '' >> CMakeLists.txt
echo 'add_dependencies( arch nirvana matrix pch )' >> CMakeLists.txt
