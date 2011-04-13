#!/bin/sh

echo 'add_library( client STATIC' > CMakeLists.txt

ls *.{hpp,cpp} | sed -e 's/^./  \0/' >> CMakeLists.txt
ls ui/*.{hpp,cpp} | sed -e 's/^./  \0/' >> CMakeLists.txt

echo ')' >> CMakeLists.txt
echo '' >> CMakeLists.txt
echo 'add_dependencies( client censor nirvana matrix pch )' >> CMakeLists.txt
