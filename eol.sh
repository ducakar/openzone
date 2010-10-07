#!/bin/sh
#
# Correct EOLs to UNIX style in all files tracked by git
#

# It's better for loop continues after errors (which are quiet likely since patterns may likely
# reference some unexistent files)
for file in `git ls-files`; do
  sed -i -e 's/\r\n/\n/g' $file
done
