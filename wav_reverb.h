#ifndef __WAV_REVERB_H
#define __WAV_REVERB_H

#define MAX_CHN		2

struct BUF_tag {
	float *buf[MAX_CHN];
};

typedef struct BUF_tag Buf;

#endif /* __WAV_REVERB_H */