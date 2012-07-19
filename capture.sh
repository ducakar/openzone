parecord -d 1 --file-format=WAVE > openzone-pa.wav &
glc-capture --pbo -s $@
kill $!
