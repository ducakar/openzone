#!/bin/sh

svn propdel -R svn:keywords .
svn propdel -R svn:ignore .

svn propset -R svn:ignore 'CMakeFiles
CMakeCache.txt
cmake_install.cmake
Makefile' src
