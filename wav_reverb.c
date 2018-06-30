#include <stdio.h>
#include <stdlib.h> 	/* malloc */
#include <stdbool.h>	/* true, false */
#include <string.h>		/* memset */
#include <sndfile.h>	/* libsndfile */
#include "wav_reverb.h"	/* data structures */
#include "process_audio.h"	/* external function prototypes */

/* local function prototypes */
bool read_input(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf);
bool write_output(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf, long frames);

int main(int argc, char *argv[])
{
	char *ifilename, *rfilename, *ofilename;
	int i, oframes;
	/* my data structures */
	Buf ibuf, rbuf, obuf;
	/* libsndfile data structures */
	SNDFILE *isfile, *rsfile, *osfile; 
	SF_INFO isfinfo, rsfinfo, osfinfo;

	/* zero libsndfile structures */
	memset(&isfinfo, 0, sizeof(isfinfo));
  	memset(&rsfinfo, 0, sizeof(rsfinfo));
  	memset(&osfinfo, 0, sizeof(osfinfo));

  	/* zero buffer pointers in Buf structures */
  	for (i=0; i<MAX_CHN; i++) {
  		ibuf.buf[i] = 0;
  		rbuf.buf[i] = 0;
  		obuf.buf[i] = 0;
  	}

	// Problem 1:
	// ToDo: Parse command line and open all files
  	if(argc!=4)
  	{
  		printf("Please input command line as ./wav_reverb ifile.wav reverb_file.wav [ofile.wav]\n");
  		return 0;
  	}
  	else
  	{
  		ifilename = argv[1];
  		rfilename = argv[2];
  		ofilename = argv[3];
  	}

  	isfile = sf_open(ifilename, SFM_READ, &isfinfo);
        if(isfile == NULL) {
      		fprintf(stderr, "Error reading source file '%s': %s\n", ifilename, sf_strerror(tmpfile));//???
      		return 1;
   		}
   	rsfile = sf_open(rfilename, SFM_READ, &rsfinfo);
        if(rsfile == NULL) {
      		fprintf(stderr, "Error reading source file '%s': %s\n", rfilename, sf_strerror(tmpfile));//???
      		return 1;
   		}

	/* Print input file information */
	printf("Input audio file %s:\n\tFrames: %d Channels: %d Samplerate: %d\n", 
		ifilename, (int)isfinfo.frames, isfinfo.channels, isfinfo.samplerate);

	printf("Impulse response file %s:\n\tFrames: %d Channels: %d Samplerate: %d\n", 
		rfilename, (int)rsfinfo.frames, rsfinfo.channels, rsfinfo.samplerate);

	/* If sample rates don't match, exit */
	if(isfinfo.samplerate!=rsfinfo.samplerate)
	{
		printf("Sample rates of input file and output file do not match\n");
		return 1;
	}

	/* If number of channels don't match or too many channels, exit */
	if(isfinfo.channels!=rsfinfo.channels)
	{
		printf("Channel number of input file and output file do not match\n");
		return 1;
	}

	/* When opening a file for writing, the caller must fill in structure members 
	* 	samplerate, channels, and format. 
	* Make these the same as input file.
	*/
	osfinfo.samplerate = isfinfo.samplerate;
	osfinfo.channels = isfinfo.channels;
	osfinfo.format = isfinfo.format;
	oframes = isfinfo.frames+rsfinfo.frames -1;
	/* open output file */
	osfile = sf_open(ofilename, SFM_WRITE, &osfinfo);

	// Problem 2:
	// ToDo: Allocate all buffers and read input into buffers

	/* Allocate separate buffers for each channel of 
	 * input, reverb and output files
	 */
	//try here!!!!!!!!!
	for (i=0; i<isfinfo.channels; i++) {
		ibuf.buf[i] = (float*)malloc(isfinfo.frames * sizeof(float));//
		rbuf.buf[i] = (float*)malloc(rsfinfo.frames * sizeof(float));//
		/* output is length of input + length of reverb -1 */
		obuf.buf[i] = (float*)malloc((isfinfo.frames+rsfinfo.frames-1) * sizeof(float));
	}
	printf("Allocated buffers\n");

	/* read interleaved data from files into de-interleaved buffers */
	/* input */
	if ( !read_input(isfile, &isfinfo, &ibuf) ) {
		fprintf(stderr, "ERROR: Cannot read input file %s", ifilename);
		return -1;
	}

	/* reverb */
	if ( !read_input(rsfile, &rsfinfo, &rbuf) ) {
		fprintf(stderr, "ERROR: Cannot read input file %s", rfilename);
		return -1;
	}
	printf("Read input files\n");

	/* process each channel of input with reverb impulse response
	 * to make output
	 */
	printf("Processing audio\n");
	process_audio(&ibuf, isfinfo.frames, isfinfo.channels, &rbuf, rsfinfo.frames, &obuf);
	printf("Finished processing audio\n");

	/* interleave output data and write output file */
	if ( !write_output(osfile, &osfinfo, &obuf, oframes) ) {
		fprintf(stderr, "ERROR: Cannot write output file %s", ofilename);
		return -1;
	}
	//write_output(osfile, &isfinfo, &ibuf, isfinfo.frames);
	
	/* Must close file; output will not be written correctly if you do not do this */
	sf_close (isfile);
	sf_close (rsfile);
	sf_close (osfile);

	/* free all buffer storage */
	printf("Freeing buffers\n");
	for (i=0; i<MAX_CHN; i++) {
		if (ibuf.buf[i] != NULL)
			free(ibuf.buf[i]);
		if (rbuf.buf[i] != NULL)
			free(rbuf.buf[i]);
		if (obuf.buf[i] != NULL)
			free(obuf.buf[i]);
	}

}

bool read_input(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf)
{
	int i, j, count;
	//float frame_buf[MAX_CHN*sfinfo->frames]; /* to hold one sample frame of audio data */
	//memset(frame_buf,0,sizeof(float)*MAX_CHN*sfinfo->frames);
	float frame_buf[MAX_CHN];
	//count = sf_read_float (sfile, frame_buf, sfinfo->channels*sfinfo->frames);
	
	for (i=0; i<sfinfo->frames; i++) {
		/* for each frame */
		memset(frame_buf,0, MAX_CHN);

		if ((count = sf_read_float (sfile, frame_buf, sfinfo->channels)) != sfinfo->channels) {
			fprintf(stderr, "Error: on sample frame %d\n", i);
			return false;}
	
		//ToDo: de-interleave the frame[j] into separate channel buffers buf->buf[j][i]
		for(j=0;j<sfinfo->channels;j++)
		{
			//buf->buf[j] = (float*)malloc(sfinfo->channels * sizeof(float));//???
			//printf("frame size: %d\n", sfinfo->frames);
			//printf("frame_buf: %f\n",frame_buf[i*sfinfo->channels +j]);
			//buf->buf[j][i] = frame_buf[i*sfinfo->channels +j];
			buf->buf[j][i] = frame_buf[j];
			//buf->buf[i*sfinfo->frames +j] = frame_buf[j];
			//printf("buf[%d][%d]: %f\n",j,i,buf->buf[j][i]);
			//printf("buf[%d][%d]: %f\n",j,i,frame_buf[i*sfinfo->channels +j]);
		}

	}
		//printf("123\n");
		return true;
}
	


bool write_output(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf, long frames)
{
	int i, j, count;
	//float frame_buf[MAX_CHN*frames]; /* to hold one sample frame of audio data */
	float frame_buf[MAX_CHN];
	
	for (i=0; i<frames; i++) {
		/* for each frame */
		//ToDo: interleave separate channel buffers buf->buf[j][i] into a frame_buf[j]
		for(j=0;j<sfinfo->channels;j++)
		{

			//frame_buf[i*sfinfo->channels +j] = buf->buf[j][i];
			
			frame_buf[j] = buf->buf[j][i];
			//printf("frame buf:%f\n",frame_buf[j]);
		}
		
		if ( (count = sf_write_float  (sfile, frame_buf, sfinfo->channels)) != sfinfo->channels) {
			fprintf(stderr, "Error: on sample frame %d\n", i);
			return false;
		}
	}
	//sf_write_float(sfile, frame_buf, sfinfo->channels*frames);

	printf("Wrote %ld frames\n", frames);
	return true;
}
