#!/bin/bash
#
# capture.sh
#
# Run OpenZone with glc tool to capture video. Audio is captured from PulseAudio server and saved
# into `openzone-pa.wav` file.
#

set -e

parecord -d 0 --file-format=WAVE >openzone-pa.wav &
glc-capture --pbo -s "$@"
kill $!
