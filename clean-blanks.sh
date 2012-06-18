#!/bin/sh

files=`git ls-files | grep -v '^include/\|^doc/licences' | grep -v 'README' | grep -v 'header\.glsl'`

# We want to remove duplicated blank lines and blank lines at the end of a file (in Unix means).
# The latter cannot be directly performed by sed, so we add additional blank line at the end of
# each file, remove duplicated blank lines and then remove one blank line at the end of each file.

for file in $files; do
  echo "" >> $file
  sed '/./,/^$/ !d' -i $file
  sed '$,$ d' -i $file
done
