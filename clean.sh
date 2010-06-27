#!/bin/sh

if [ -f Makefile ]; then
  echo 'make clean'
  make clean
fi

echo 'rm -rf *.gcno src/*/*.gcno'
rm -rf *.gcno src/*/*.gcno
echo 'rm -rf CMakeCache.txt'
rm -rf CMakeCache.txt
echo 'rm -rf CMakeFiles cmake_install.cmake Makefile'
rm -rf CMakeFiles cmake_install.cmake Makefile
echo 'rm -rf src/CMakeFiles src/cmake_install.cmake src/Makefile'
rm -rf src/CMakeFiles src/cmake_install.cmake src/Makefile
echo 'rm -rf src/*/CMakeFiles src/*/cmake_install.cmake src/*/Makefile'
rm -rf src/*/CMakeFiles src/*/cmake_install.cmake src/*/Makefile
echo 'rm -f src/oz/CMakeLists.txt src/matrix/CMakeLists.txt src/nirvana/CMakeLists.txt src/ui/CMakeLists.txt src/client/CMakeLists.txt src/server/CMakeLists.txt'
rm -f src/oz/CMakeLists.txt src/matrix/CMakeLists.txt src/nirvana/CMakeLists.txt src/ui/CMakeLists.txt src/client/CMakeLists.txt src/server/CMakeLists.txt
echo 'rm -f src/configuration.hh'
rm -f src/configuration.hh
echo 'rm -f *.kdevelop.filelist *.kdevelop.pcs *.kdevses'
rm -f *.kdevelop.filelist *.kdevelop.pcs *.kdevses
echo 'rm -rf doxygen'
rm -rf doxygen
