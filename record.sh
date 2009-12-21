#!/bin/sh

if [ -z "$1" ]; then
  echo usage: $0 file
fi

src/client/openzone &
sleep 3
aoss ffmpeg -f oss -i /dev/dsp -f x11grab -s 640x360 -r 20 -i :0 -b 800k -y "$1"
