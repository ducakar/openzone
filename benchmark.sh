#!/bin/sh

[[ -z "$1" ]] && exit

cpus=`cat /proc/cpuinfo | grep '^processor' | sed -e 's/.*\(.\)$/\1/'`

( cd $1 && make openzone -j4 )

for cpu in $cpus; do
  sudo cpufreq-set -c $cpu -g powersave
done

$1/src/client/openzone -i test -t 60

for cpu in $cpus; do
  sudo cpufreq-set -c $cpu -g ondemand
done
