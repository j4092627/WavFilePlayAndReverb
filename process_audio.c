#include <stdio.h>
#include <math.h>
#include <sndfile.h>	/* libsndfile */
#include "wav_reverb.h"
#include "convolve.h"	/* FFT-based convolution */

#define MODE	2

void process_audio(Buf *ibuf, int iframes, int channels, Buf *rbuf, int rframes, Buf *obuf)
{
#if (MODE == 1)
	/* just copy input to output */
	int i, j;
	float *ip, *rp;
	//printf("processing\n");
	ip = (float*)malloc(iframes*sizeof(float));
	rp = (float*)malloc(rframes*sizeof(float));
	for (i=0; i<channels; i++) {
		ip = ibuf->buf[i];
		rp = rbuf->buf[i];
		//printf("ip: %f",ibuf->buf[i]);
		
		for (j=0; j<iframes; j++) {
			//printf("ip[%d]:%f\n",j,ip[j]);
			obuf->buf[i][j] = ip[j];
		}
		for ( ; j<iframes+rframes-1; j++) {
			obuf->buf[i][j] = 0;
		}
		//printf("processing\n");
	}
#else
#if (MODE == 2)	
	/* do reverberation via time-domain convolution */
	int i, j, k, oframes;
	float *ip, *rp;
	double v, gain, rms_iv, rms_ov;
	//float rms_iv, rms_ov;
	/* set initial values */
	rms_iv = 0;
	rms_ov = 0;
	ip = (float*)malloc(iframes*sizeof(float));
	rp = (float*)malloc(rframes*sizeof(float));
	// ToDo: convolve each channel signal with reverb impulse response signal
	for(i=0;i<channels;i++)
	{
		ip = ibuf->buf[i];
		rp = rbuf->buf[i];
		for(j=0;j<iframes;j++)
		{
			obuf->buf[i][j] = 0;
			//rms_iv = (float)rms_iv + fabs(ip[j]);
			if(rms_iv < fabs(ip[j]))
			{
				rms_iv = fabs(ip[j]);
			}
			//printf("ip:%f\n",ip[j]);
			for(k=0;k<rframes;k++)
			{
				if((j-k)>=0)
				{
					obuf->buf[i][j] += ip[j-k]*rp[k]; 
					if(rms_ov < fabs(obuf->buf[i][j]))
					{
						rms_ov = obuf->buf[i][j];
					}
					//rms_ov = rms_ov + fabs(obuf->buf[i][j]);
				}
			}
		}
	}
	//scale to make output rms value be equal to input rms value
	//rms_iv = sqrt(rms_iv);
	//rms_ov = sqrt(rms_ov);
	gain = rms_iv/rms_ov;
	
	for(i=0;i<channels;i++)
	{
		for(j=0;j<iframes;j++)
		{
			obuf->buf[i][j] = obuf->buf[i][j]*gain;
		}
	}




#else /* (MODE == 3) */
	/* do reverberation via frequency-domain convolution */
	int i;
	/* for each channel */
	for (i=0; i<channels; i++) {
		/* convolve channel signal with reverb impulse response signal */
		/* frequency domain (FFT) convolution */
		convolve(ibuf->buf[i], rbuf->buf[i], iframes, rframes, obuf->buf[i]);
	}
#endif
#endif
}
