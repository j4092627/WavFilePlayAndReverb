#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
	FILE *ifp, *ofp;
	unsigned char header[44];
	char c;
	int fsamp,channel_num,bits_rate,file_size;
	int target_val = atoi(argv[2]);
	char max[4] = {'-','m','a','x'};


	if(argc!=5)
	{
		printf("The command line is error.\nEnter command line as: ./process_wav –max amplitude ifile ofile\n");
	}
	else if((ifp = fopen(argv[3],"r"))==NULL)
	{
		printf("Input file is not found.\n");
		printf("Enter command line as: ./process_wav –max amplitude ifile ofile\n");
	}
	else if(fread(header, sizeof(header),1,ifp)!=1)
	{
		printf("The file is not found\n");
	}

	else if(strncmp(argv[1],max,strlen(argv[1]))!=0)
	{
		printf("The command line is error.\nEnter command line as: ./process_wav –max amplitude ifile ofile\n");
	}
	else if(target_val <0 || target_val >32768)
	{
		printf("please enter amplitude number between 0~32768\n");
	}
	else
	{
		ofp = fopen(argv[4],"w");
		for(int i=1; i< sizeof(header); i++)
		{
			
			if(isalpha(header[i-1])==1)
			{
				c = header[i-1];
				printf("%c %02x ", c, header[i-1]);
			}
			else
			{
				printf(" ");
			}
			if(i%11 == 0)
			{
				printf("\n");
			}
		}
		printf("\n");
		fwrite(header , sizeof(header) , 1 , ofp );

		
		fsamp = header[27]<<24 | header[26]<<16 | header[25]<<8 | header[24];
		printf("Sample Rate: %d\n",fsamp);
		channel_num = header[23]<<8 | header[22];
		printf("Channel Num: %d\n",channel_num);
		bits_rate = header[35]<<8 | header[34];
		printf("Bits per Sample: %d\n",bits_rate);
		file_size = header[43]<<24 | header[42]<<16 | header[41]<<8 | header[40];
		printf("File size: %d\n",file_size);

		short *x;
		if ( (x = (short *)malloc(file_size)) == NULL) 
		{
		// error reporting

		}
		if ( fread(x, sizeof(*x), file_size/sizeof(short), ifp) != 1 ) 
		{
		// error reporting

		}
		
		int vmax = 0;
		for (int i=0; i<file_size/sizeof(short); i++) 
		{
			//printf("%d\n",x[i] );
			if (abs(x[i]) > vmax) 
			{
				vmax = abs(x[i]);
			}
		}
		//printf("vmax:%d\n",vmax);

		short *x_new;
		x_new = (short *)malloc(file_size);

		//printf("float: %f\n",(float)target_val / vmax);
		for(int i=0; i<file_size/sizeof(short); i++)
		{
			float count;
			x_new[i] = (float)x[i] *target_val / vmax;
			
			//printf("%02x\n",x_new[i]);
			printf("%d\n",x_new[i]);
			//fprintf(ofp,"%02x",x_new[i]);

		}
		fwrite(x_new , sizeof(*x_new) , file_size/sizeof(short) , ofp );
	}
}
