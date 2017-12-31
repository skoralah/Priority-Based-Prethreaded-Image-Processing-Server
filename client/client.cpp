extern "C"
{
#include "csapp.h"
}
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <highgui.h>
#include <iostream>
using namespace cv;
int main(int argc, char **argv) 
{
    int clientfd, port, n;
    char *host; 
    rio_t rio;

    if (argc != 3) {
	fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
	exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);
    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);
	
   FILE *fp = fopen("college.png","rb");
        if(fp==NULL)
        {
            printf("File open error");
            return 1;   
        }  

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);

   
    Rio_writen(clientfd, &size, sizeof(size));

    fseek(fp, 0, SEEK_SET);

    while(1)
    {
      
        unsigned char buff[256];
	int nread = fread(buff,1,256,fp);
        printf("File read :%d bytes\n", nread);        

       
        if(nread > 0)
        {
            Rio_writen(clientfd, buff, nread);
            printf("Sending %d\n",nread);
            
        }

        
        if (nread < (256))
        {
            if (feof(fp))
                printf("End of file\n");
            if (ferror(fp))
                printf("Error reading\n");
            break;
        }



    }
    fclose(fp);

	    int rx_fsize;
	    int bytesReceived = 0; 
	    char recvBuff[256]; 
	    Rio_readnb(&rio,&rx_fsize,sizeof(rx_fsize));
	    
	    printf("Size of rx file %d\n",rx_fsize );

	    FILE *rx_fp;
	    rx_fp = fopen("college_gray.png", "wb");

	    if(NULL == rx_fp)
	    {
		  printf("Error opening file");
		  return -1;
	    }

	   
	    fseek(rx_fp, 0, SEEK_SET);

	    int recData = 256;

	    
	    while(((bytesReceived = Rio_readnb(&rio,recvBuff,recData))>0) && rx_fsize>0)
	    {
		  printf("Bytes received %d\n",bytesReceived);    
		  
		  fwrite(recvBuff, 1,bytesReceived,rx_fp);
		  
		  rx_fsize -= bytesReceived;
		  if (rx_fsize<256)       
			recData = rx_fsize;
	    }

	    fclose(rx_fp);
	    printf("File received\n");
	    
	    Close(clientfd); 
	    
	    Mat img = imread("college_gray.png", CV_LOAD_IMAGE_UNCHANGED);
	    namedWindow( "college_gray.png", CV_WINDOW_AUTOSIZE );
	    imshow( "college_gray.png", img );

    	    Mat img1 = imread("college.png", CV_LOAD_IMAGE_UNCHANGED);
	    namedWindow( "college.png", CV_WINDOW_AUTOSIZE );
	    imshow( "college.png", img1 );

	    waitKey(0);
	    destroyWindow("college_gray.png");
	    destroyWindow("college.png");
	    exit(0);
    }

   
