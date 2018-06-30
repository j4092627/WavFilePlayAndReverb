gcc -g -std=c99 -Wall -I/usr/local/include \
    -o wav_reverb wav_reverb.c process_audio.c convolve.c \
    -L/usr/local/lib -lsndfile
