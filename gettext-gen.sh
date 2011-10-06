#!/bin/sh

sources='src/*/*.hpp src/*/*/*.hpp src/*/*.cpp src/*/*/*.cpp'
classes='share/openzone/class/*.rc'
output='share/locale/openzone.pot'

rm -rf $output
xgettext --omit-header -c++ -s -d openzone -o $output $sources

echo >> $output
echo '#' >> $output
echo '# share/class/*.rc' >> $output
echo '#' >> $output

for class_path in $classes; do
  class=`basename $class_path .rc`

  if ( grep '^title' $class_path &> /dev/null ); then
    class=`grep -h '^title' $class_path | sed 's/[^"]*"\(.*\)"[^"]*$/\1/'`
  fi

  # add class name if it doesn't exist yet
  if ( grep "^msgid \"$class\"" $output &> /dev/null ); then
    echo &> /dev/null;
  else
    echo >> $output
    echo "msgid \"$class\"" >> $output
    echo "msgstr \"\"" >> $output
  fi
done

# extract weapon names
weapons=`grep -h '^weapon[0-9][0-9]\.name' $classes | \
    sed 's/[^"]*"\(.*\)"[^"]*/\1/' | \
    sed 's/ /_/g'`

for weapon in $weapons; do
  weapon=`echo $weapon | sed 's/_/ /g'`
  # add weapon name if it doesn't exist yet
  if ( grep "^msgid \"$weapon\"" $output &> /dev/null ); then
    echo &> /dev/null;
  else
    echo >> $output
    echo "msgid \"$weapon\"" >> $output
    echo "msgstr \"\"" >> $output
  fi
done

echo >> $output
echo '#' >> $output
echo '# share/lua/*/*.lua' >> $output
echo '#' >> $output

lua_src="share/openzone/lua/*/*.lua"

for lua_file in $lua_src; do
  cat $lua_file | grep ' *ozGettext( ".*" )[,]*$' | \
      sed 's/ *ozGettext( "\(.*\)" )$/\nmsgid "\1"\nmsgstr ""/' >> $output
done
