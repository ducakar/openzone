#!/bin/sh

echo 'add_library( nirvana STATIC' > CMakeLists.txt

ls *.{hpp,cpp} | sed -e 's/^./  \0/' >> CMakeLists.txt

echo ')' >> CMakeLists.txt
echo '' >> CMakeLists.txt
echo 'add_dependencies( nirvana matrix pch )' >> CMakeLists.txt
