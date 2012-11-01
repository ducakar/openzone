#!/bin/sh

sed \
'/#BEGIN SOURCES/,/#END SOURCES/ c\#BEGIN SOURCES\n'"\
  `echo *.{hh,cc} */*.{hh,cc} | sed 's| |\\\\n  |g'`\
"'\n#END SOURCES' -i CMakeLists.txt
