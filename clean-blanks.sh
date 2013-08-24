#!/bin/sh
#
# clean-blanks.sh
#
# Cleans up trailing blanks, duplicated empty lines and missing newlines at the end of file for most
# files in the source tree.
#
# We want to remove duplicated blank lines and blank lines at the end of a file (in Unix means).
# The latter cannot be directly performed by sed, so we add additional two blank lines at the end of
# each file, remove duplicated blank lines and then remove the last (always blank) line.
#

files=`git ls-files | egrep -v '^doc/licences/|^etc/include/|^etc/android-project/|\.patch$|\.po$'`

for file in $files; do
  # Add two blank lines to the end of each file.
  printf '\n\n' >> $file
  # Remove trailing blanks and then remove duplicated empty lines.
  sed -r 's|[ \t]*$||; /./,/^$/ !d' -i $file
  # Delete the last (always blank) line.
  sed -r '$ d' -i $file
done
