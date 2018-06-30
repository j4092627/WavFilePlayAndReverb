DIR=signals/mono
ifile=$DIR/svega.wav

time=`infoaudio $ifile | grep "Number of samples" | awk '{print $6 $7}'`
echo
echo $ifile is $time
echo
time ./wav_reverb $ifile $DIR/medieval_church.wav foo.wav
