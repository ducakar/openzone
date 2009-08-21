#!/bin/sh

rm -rf pogon.*

src/client/openzone &
sleep 3
ffmpeg -f x11grab -s 640x480 -r 15 -i :0 -vcodec rawvideo pogon.ffm &
arecord -f dat pogon.au &
sleep 300
killall ffmpeg arecord
sleep 1
killall openzone
