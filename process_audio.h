#ifndef __PROCESS_AUDIO
#define __PROCESS_AUDIO

void process_audio(Buf *ibuf, int iframes, int channels, Buf *rbuf, int rframes, Buf *obuf);

#endif /* __PROCESS_AUDIO */