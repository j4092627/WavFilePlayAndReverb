/*****************************************************************************
 * play_wavfiles.c
 *
 * Plays one of a set of WAV files to speaker using PortAudio
 * Selection is via keyboard input using Ncurses
 * Wav file access uses Sndfile
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h> 	/* malloc() */
#include <unistd.h>     /* sleep() */
#include <stdbool.h>	/* true, false */
#include <string.h>		/* memset() */
#include <ctype.h>		/* lolower() */
#include <math.h>		/* sin() */
#include <sndfile.h>	/* libsndfile */
#include <portaudio.h>	/* portaudio */
#include <ncurses.h> 	/* This library is for getting input without hitting return */

#define MAX_PATH_LEN        256
#define MAX_IFILES		    8
#define MAX_CHN	            2
#define FRAMES_PER_BUFFER   512 /* must be divisible by 2 -- see fade-out window code */
#define PI		            3.14159265358979323846

/* data structure to pass to callback */
struct BUF_tag {
    /* libsndfile data structures */
    SNDFILE *sndfile[MAX_IFILES]; 
    SF_INFO sfinfo[MAX_IFILES];
	unsigned int num_chan;
    int selection;
};
typedef struct BUF_tag Buf;

/* PortAudio callback function protoype */
static int paCallback( const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );

int main(int argc, char *argv[])
{
	char ifilename[MAX_IFILES][MAX_PATH_LEN], ch;
	int i, selection;
	unsigned int num_input_files, samp_rate, num_chan;
	FILE *fp;
	/* my data structure */
	Buf iBuf, *p = &iBuf;
	/* PortAudio data structures */
    PaStream *stream;
    PaError err;
    PaStreamParameters outputParams;
    PaStreamParameters inputParams;

	/* zero libsndfile structures */
	for (i=0; i<MAX_IFILES; i++) {
		memset(&p->sfinfo[i], 0, sizeof(p->sfinfo[i]));
	}

	/* 
	 * Parse command line and open all files 
	 */
  	//YOUR CODE HERE
	if(argc < 2)
	{
		printf("Please enter command line as ./play_wavfiles ifile_list.txt\n");
		return 0;
	}
	if((fp = fopen(argv[1], "r"))==NULL)
	{
		printf("File is not found\n");
		return 0;
	}
  	/* open list of files */
    //YOUR CODE HERE
    i = 0;
    while((fgets(ifilename[i],MAX_PATH_LEN,fp))!=NULL)
    {
    	int len = strlen(ifilename[i]);
    	ifilename[i][len-1] = 0;
    	printf("%d %s\n",i, ifilename[i]);
    	i++;
    }
    num_input_files = i;

  	for (i=0; i<num_input_files ; i++) {
  		/* read file from list */
        //YOUR CODE HERE
        
  		/* open file */
        //YOUR CODE HERE
  		iBuf.sndfile[i] = sf_open(ifilename[i], SFM_READ, &iBuf.sfinfo[i]);
        if(iBuf.sndfile[i] == NULL) {
      		fprintf(stderr, "Error reading source file '%s': %s\n", argv[1], sf_strerror(tmpfile));//???
      		return 1;
   		}

		/* Print file information */
		printf("%d Frames: %8d, Channels: %d, Samplerate: %d, %s\n",  
			i, (int)p->sfinfo[i].frames, p->sfinfo[i].channels, p->sfinfo[i].samplerate, ifilename[i]);
 	}
 	num_input_files = i;
 

 	/* check for compatibility of input files */
 	samp_rate = p->sfinfo[0].samplerate;
 	for (i=1; i<num_input_files; i++) {
	 	/* If sample rates don't match, exit */
		if ( samp_rate != p->sfinfo[i].samplerate) {
			printf("Error: audio input sample rates must be the same\n");
			return -1;
		} 		
		/* If number of channels don't match or too many channels, exit */
		num_chan = p->sfinfo[0].channels;
		if ( num_chan != p->sfinfo[i].channels) {
			printf("Error: number of audio input channels must be the same.\n");
			return -1;
		} 
 	}
 	for (i=1; i<num_input_files; i++) {
 		/* if too many channels, exit */
		if ( p->sfinfo[i].channels > MAX_CHN ) {
			printf("Error: number of input channels %d greater than max channels %d.\n",
				p->sfinfo[i].channels, MAX_CHN);
			return -1;
		} 
	}

	/* initialize structure */
	p->num_chan = num_chan;
    /* initially, don't play any file */
    p->selection = -1;

    /* pause so user can read console printout */
    sleep(1);

	/* Initializing PortAudio */
    err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        return -1;
    }
    
    /* Input stream parameters */
    inputParams.device = Pa_GetDefaultInputDevice();
    inputParams.channelCount = 1;
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency =
        Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

    /* Ouput stream parameters */
    outputParams.device = Pa_GetDefaultOutputDevice();
    outputParams.channelCount = 2;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency =
        Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;
    
    /* Open audio stream */
    err = Pa_OpenStream(&stream,
        &inputParams, /* no input */
        &outputParams,
        samp_rate, FRAMES_PER_BUFFER,
        paNoFlag, /* flags */
        paCallback,
        &iBuf);
    
    if (err != paNoError) {
        printf("PortAudio error: open stream: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        return -1;
    }
    
     /* Start audio stream */
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        printf(  "PortAudio error: start stream: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        return -1;
    }


	/* Initialize ncurses 
     * to permit interactive character input 
     */
	initscr(); /* Start curses mode */
	cbreak();   //Line buffering disabled 
	noecho(); /* Uncomment this if you don't want to echo characters when typing */  

    printw("Select input file by number:\n");
    //YOUR CODE HERE
    

	refresh();

	ch = '\0'; /* Init ch to null character */
	while (ch != 'q') {
		ch = tolower(getch()); 
		if (ch >= '0' && ch < '0'+num_input_files) {
			selection = ch-'0';
            /* write information to be read in callback
             * this "one-way" communication of a single item
             * eliminated the possibility of race condition due to
             * asynchronous threads
             */
            p->selection = selection;
		}
        mvprintw(6, 0, "Playing %d, New selection: ", selection);
        refresh();
	}

	/* End curses mode  */
	endwin();

    /* Stop stream */
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        printf(  "PortAudio error: stop stream: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        exit(1);
    }
    
    /* Close stream */
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        printf(  "PortAudio error: close stream: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        exit(1);
    }
    
    /* Terminate PortAudio */
    err = Pa_Terminate();
    if (err != paNoError) {
        printf("PortAudio error: terminate: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        exit(1);
    }
    
	/* Close files */
	for (i=0; i<num_input_files; i++) {
		sf_close (p->sndfile[i]);
	}

    return 0;
}


/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * in the routine that requires significant resources.
 */
static int paCallback(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    Buf *p = (Buf *)userData; /* Cast data passed through stream to our structure. */
    float *output = (float *)outputBuffer;
    //float *input = (float *)inputBuffer; /* input not used in this code */
    unsigned int num_samples = framesPerBuffer * p->num_chan; /* number or samples in buffer */
    unsigned int selection, i, count;
    sf_count_t num_read;//????

 
    selection = p->selection;
    if (selection == -1) {
        /* zero output buffer */
        for (i=0; i<num_samples; i++) {
            output[i] = 0.0;
        }
    }
    else {
     	/* read a buffer of input data
     	 * count is number of samples read 
         */
    	//YOUR CODE HERE

        /* clear output buffer */
		memset(output, 0, sizeof(float) * framesPerBuffer * p->num_chan);
		/* read directly into output buffer */
		num_read = sf_read_float(p->sndfile[selection], output, framesPerBuffer * p->num_chan);
		

    }

    return 0;
}
