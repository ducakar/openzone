#!/bin/sh

[[ -z "$1" ]] && exit

pkg="$1"
scripts=pkg/src/$pkg/lua/*/*.lua
bsps=pkg/src/$pkg/data/maps/*.rc
classes=pkg/src/$pkg/class/*.rc
output=pkg/src/$pkg/lingua/$pkg.pot

mkdir -p pkg/build/$pkg/lingua
rm -rf pkg/build/$pkg/lingua/*.pot

for i in $scripts; do
  xgettext --omit-header -C -s -kozGettext -d openzone -o pkg/build/$pkg/lingua/`basename $i .lua`.pot $i
done

echo >> $output
echo '#' >> $output
echo '# BSP names' >> $output
echo '#' >> $output

for bsp_path in $bsps; do
  bsp=`basename $bsp_path .rc`

  if ( grep '^title' $bsp_path &> /dev/null ); then
    bsp=`grep -h '^title' $bsp_path | sed 's/[^"]*"\(.*\)"[^"]*$/\1/'`
  fi

  # add bsp name if it doesn't exist yet
  if ( grep "^msgid \"$bsp\"" $output &> /dev/null ); then
    echo &> /dev/null;
  else
    echo >> $output
    echo "msgid \"$bsp\"" >> $output
    echo "msgstr \"\"" >> $output
  fi
done

echo >> $output
echo '#' >> $output
echo '# Class names' >> $output
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

echo >> $output
echo '#' >> $output
echo '# Vehicle weapon names' >> $output
echo '#' >> $output

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
