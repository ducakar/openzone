#!/bin/sh

echo 'add_library( matrix STATIC' > CMakeLists.txt

ls *.{hpp,cpp} | sed -e 's/^./  \0/' >> CMakeLists.txt

echo ')' >> CMakeLists.txt
echo '' >> CMakeLists.txt
echo 'add_dependencies( matrix pch )' >> CMakeLists.txt
