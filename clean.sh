#!/bin/sh

if [ -f Makefile ]; then
  echo 'make clean'
  make clean
fi

echo 'rm -rf *.gcno src/base/*.gcno src/matrix/*.gcno src/nirvana/*.gcno src/client/*.gcno src/server/*.gcno src/test/*.gcno'
rm -rf *.gcno src/base/*.gcno src/matrix/*.gcno src/nirvana/*.gcno src/client/*.gcno src/server/*.gcno src/test/*.gcno
echo 'rm -rf CMakeCache.txt'
rm -rf CMakeCache.txt
echo 'rm -rf CMakeFiles cmake_install.cmake Makefile'
rm -rf CMakeFiles cmake_install.cmake Makefile
echo 'rm -rf src/CMakeFiles src/cmake_install.cmake src/Makefile'
rm -rf src/CMakeFiles src/cmake_install.cmake src/Makefile
echo 'rm -rf src/*/CMakeFiles src/*/cmake_install.cmake src/*/Makefile'
rm -rf src/*/CMakeFiles src/*/cmake_install.cmake src/*/Makefile
echo 'rm -f src/base/CMakeLists.txt src/matrix/CMakeLists.txt src/nirvana/CMakeLists.txt src/ui/CMakeLists.txt src/client/CMakeLists.txt src/server/CMakeLists.txt'
rm -f src/base/CMakeLists.txt src/matrix/CMakeLists.txt src/nirvana/CMakeLists.txt src/ui/CMakeLists.txt src/client/CMakeLists.txt src/server/CMakeLists.txt
echo 'rm -f *.kdevelop.filelist *.kdevelop.pcs *.kdevses'
rm -f *.kdevelop.filelist *.kdevelop.pcs *.kdevses
echo 'rm -rf doxygen'
rm -rf doxygen
