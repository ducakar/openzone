parecord -d 0 --file-format=WAVE > openzone-pa.wav &
glc-capture --pbo -s $@
kill $!
