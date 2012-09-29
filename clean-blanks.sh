#!/bin/sh
#
# We want to remove duplicated blank lines and blank lines at the end of a file (in Unix means).
# The latter cannot be directly performed by sed, so we add additional two blank lines at the end of
# each file, remove duplicated blank lines and then remove one blank line at the end of each file.
#

files=`git ls-files | \
       grep -v '^include/\|^doc/licences\|\.patch$\|\.po$' | \
       grep -v 'README' | \
       grep -v 'header\.glsl'`

for file in $files; do
  # Remove duplicated empty lines.
  printf '\n\n' >> $file
  sed '/./,/^$/ !d' -i $file
  sed '$,$ d' -i $file

  # Remove trailing blanks at the end of line.
  sed 's/[ \t]*$//' -i $file
done
