extern "C"
{
#include "csapp.h"
}
#include <stdio.h>
#include <iostream>
char imageRcvd = 1;
int processimage();

void echo(int connfd) 
{
	size_t n;
	int bytesReceived = 0, result; 
	char recvBuff[256]; 
	rio_t rio;

	Rio_readinitb(&rio, connfd);
	int rx_fsize;
	
	
	Rio_readnb(&rio,&rx_fsize,sizeof(rx_fsize));
	
	//printf("Size of file %d\n",rx_fsize );
	FILE *fp;
	fp = fopen("college1.png", "wb");
	if(NULL == fp)
	{
		printf("Error opening file");
		return;
	}
	
	fseek(fp, 0, SEEK_SET);
	int recData = 256;
	
	while(((bytesReceived = Rio_readnb(&rio,recvBuff,recData))>0) && rx_fsize>0)
	{
		//printf("Bytes received %d\n",bytesReceived);    
		
		fwrite(recvBuff, 1,bytesReceived,fp);
		
		rx_fsize -= bytesReceived;
		//printf("remaining bytes :%d\n",rx_fsize);
		if (rx_fsize<256)	
        	recData = rx_fsize;
	}
	fclose(fp);
	//printf("File received\n");
	
	processimage();
		FILE *fp2;
		fp2 = fopen("Gray_college.png", "rb");
		if(NULL == fp2)
		{
			printf("Error opening file");
			return;
		}
		fseek(fp2, 0, SEEK_END);
		int tx_size = ftell(fp2);
		fseek(fp2, 0, SEEK_SET);
		//printf("tx_size = %d\n",tx_size);

		
		Rio_writen(connfd, &tx_size, sizeof(tx_size));
		fseek(fp2, 0, SEEK_SET);

		
		while(1)
		{
			
			//printf("%d\n",size );
			unsigned char buff[256];

			int nread = fread(buff,1,256,fp2);
			//printf("File read :%d bytes\n", nread);        

			
			if(nread > 0)
			{
				//printf("Sending back\n");
            
				Rio_writen(connfd, buff, nread);
			}

			
			if (nread < (256))
			{
				if (feof(fp2))
					printf("End of file\n");
				if (ferror(fp2))
					printf("Error reading\n");
				break;
			}
		}
		fclose(fp2);
	
	
	}
