#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <highgui.h>
#include <iostream>
using namespace cv;
int processimage( )
{
 

 Mat image;
 image = imread( "college1.png", 1 );

 if( !image.data )
 {
   printf( " No image data \n " );
   return -1;
 }

 Mat gray_image;
 cvtColor( image, gray_image, CV_BGR2GRAY );

 imwrite( "Gray_college.png", gray_image );

 return 0;
}

