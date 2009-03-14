#!/bin/sh

rm pogon.*

src/client/dark &
sleep 3
ffmpeg -f x11grab -s 320x240 -r 15 -i :0 -vcodec rawvideo pogon.ffm &
arecord -f cd pogon.au &
sleep 300
killall ffmpeg arecord
sleep 1
killall dark
