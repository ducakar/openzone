#!/bin/sh
#
# Screen & audio capture
# It captures 640x350 (mini HD) in the top left corner, so you have to properly adjust resolution
# for openzone and window manager (e.g. KWin can do it) settings to open openzone window without any
# borders and in the top-left corner.
# First parameter: file name where the data should be dumped/encoded. FFMpeg deduces format for
# encoding from the file ending, read FFMpeg manual for details and/or adjust parameters in the
# last line of the script.
#

if [ -z "$1" ]; then
  echo usage: $0 file
fi

src/client/openzone &
sleep 3
aoss ffmpeg -f oss -i /dev/dsp -f x11grab -s 640x360 -r 20 -i :0 -b 800k -y "$1"
