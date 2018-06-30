# Compile and link program to play multiple files

gcc -Wall play_wavfiles.c -o play_wavfiles -I/usr/local/include \
    -L/usr/local/lib -lsndfile -lportaudio -lncurses
