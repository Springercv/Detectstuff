// OpenCV_Helloworld.cpp : Defines the entry point for the console application.
// Created for build/install tutorial, Microsoft Visual Studio and OpenCV 2.2.0
#include <iostream>  
#include "stdafx.h"
#include <stdio.h>   
#include <time.h>  
#include <cv.h>   
#include <cxcore.h>   
#include <highgui.h>   
#include<windows.h>   
#include<malloc.h>   
#include <stdio.h>   
#include<math.h>   


void Color(IplImage *img);  
//void Color(IplImage *img);   
void detect_and_draw( IplImage* img);//, IplImage* imgshow );
   
struct num   
    {   
        unsigned char H;   
        unsigned char S;   
        unsigned char V;   
    };   

const char* cascade_name =
    "C:/INI/Subversion/OpenCV/data/haarcascades/haarcascade_frontalface_alt.xml";    
   
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

/*int main()
{
    Mat src = imread("people.jpg");
    if (src.empty())
        return -1;

    Mat hsv;
    cvtColor(src, hsv, CV_BGR2HSV);

    Mat bw;
    inRange(hsv, Scalar(0, 58, 89), Scalar(25, 173, 229), bw);

    imshow("src", src);
    imshow("dst", bw);
    waitKey(0);

    return 0;
}*/
int _tmain(int argc, _TCHAR* argv[])
{
        // Open the file.
    
    IplImage* pFrame = NULL;   
    CvCapture* pCapture = NULL;  
    double  t = 0, total = 0, time = 0;   int numFrames = 0;
    //cvNamedWindow("video",1);  
    
    pCapture = cvCaptureFromCAM(-1);  
    if(pCapture)   
      {   
        for(;;)   
        {   
            pFrame = cvQueryFrame(pCapture);   
            t = (double)cvGetTickCount();
            Color(pFrame);   
            //cvShowImage("skin video",pFrame);
                    //  Color_Guass(pFrame);   
            //cvShowImage( "result1", pFrame );
            detect_and_draw(pFrame);   
            t = (double)cvGetTickCount()-t; 
            time = t/((double)cvGetTickFrequency()*1000.); 
            total += time;
            numFrames ++; 
            printf( "detection time = %g ms\n", time );
            //printf( "average processing speed = %g fms\n", total/numFrames);
            //cvShowImage("Video face detection by Xuan Le - Neuroinformatik, Ruhr-Uni-Bochum, Germany",pFrame);   
            if(cvWaitKey(10)>=0)   
            break;   
        }   
        cvReleaseCapture(&pCapture);   
       } 
    else
    {
      pFrame = cvLoadImage("D:/lexuanpr/Datasets/Caltech/image_0003.jpg");
      Color(pFrame); 
      //detect_and_draw(pFrame);   
      t = (double)cvGetTickCount()-t; 
      time = t/((double)cvGetTickFrequency()*1000.); 
      total += time;
      numFrames ++; 
      printf( "detection time = %g ms\n", time );
      //printf( "average processing speed = %g fms\n", total/numFrames);
      cvShowImage("Video face detection by Xuan Le - Neuroinformatik, Ruhr-Uni-Bochum, Germany",pFrame);   
      cvWaitKey(0);
    }
    //cvDestroyWindow("video");   
    cvReleaseImage(&pFrame);
return 0;     
}
void Color(IplImage *img)   
{       
    int i,j;   
    IplImage *img_hsv = 0;   
    img_hsv = cvCreateImage(cvGetSize(img),8,3);   
    cvCvtColor(img,img_hsv,CV_BGR2HSV);   
    struct num **bmpdata;   
    struct num **bmpdata1;   
    bmpdata = new num*[img->height];   
    bmpdata1 = new num*[img->height];   
   
    for(i=0;i<img->height;i++)   
    {   
        bmpdata[i] = new num[img->width];   
        bmpdata1[i] = new num[img->width];      
    }   
   
    for(i=0;i<img->height;i++)     // Access image address and read data using widthStep from OpenCV
        for(j=0;j<img->width;j++)   
        {   
            bmpdata[i][j].H=((uchar*)(img_hsv->imageData + img_hsv->widthStep*i))[j*3];   
            bmpdata[i][j].S=((uchar*)(img_hsv->imageData + img_hsv->widthStep*i))[j*3+1];   
            bmpdata[i][j].V=((uchar*)(img_hsv->imageData + img_hsv->widthStep*i))[j*3+2];   
        }  
    for (i=0;i<img->height;i++)   
       {   
          for (j=0;j<img->width;j++)   
          {    
           if(bmpdata[i][j].H<=15&&bmpdata[i][j].S>=48)   
            bmpdata[i][j].H+=0;   
            else    bmpdata[i][j].H=bmpdata[i][j].S=bmpdata[i][j].V=0;   
          }   
    }   
    
   
  /* for (i=1;i<img->height-1;i++)      //
     for (j=1;j<img->width-1;j++)   
     {   
         if(bmpdata[i][j].H!=0)   
             if(bmpdata[i][j-1].H==0||bmpdata[i][j+1].H==0||   
                  bmpdata[i+1][j].H==0||bmpdata[i-1][j].H==0   
                 ){   
               bmpdata1[i][j].H=0;   
               bmpdata1[i][j].S=0;   
               bmpdata1[i][j].V=0;   
             }   
             else{   //keep the same thing or add nothing
               bmpdata1[i][j].H+=0;   
               bmpdata1[i][j].S+=0;   
               bmpdata1[i][j].V+=0;   
             }      
     }      */
   
 for (i=0;i<img->height;i++)        // Access image address and append data using widthStep from OpenCV
     for (j=0;j<img->width;j++)   
     {      
           
                ((uchar*)(img_hsv->imageData + img_hsv->widthStep*i))[j*3]=bmpdata[i][j].H;   
                ((uchar*)(img_hsv->imageData + img_hsv->widthStep*i))[j*3+1]=bmpdata[i][j].S;   
                ((uchar*)(img_hsv->imageData + img_hsv->widthStep*i))[j*3+2]=bmpdata[i][j].V;              
     }    
     //use cvResize to resize source to a destination image
    //IplImage *dst = cvCreateImage( cvSize((int)((img->width*50)/100), 
    //(int)((img->height*50)/100) ), img->depth, img->nChannels );  
    //cvShowImage( "src", img ); 
    cvCvtColor(img_hsv,img,CV_HSV2BGR);   
    cvErode(img, img,NULL,1);   
    cvDilate(img, img,NULL,1); 
    cvReleaseImage(&img_hsv); 

    //cvResize(img, dst);
    //cvShowImage( "result1", dst );
    //img=dst;     
    //cvShowImage( "result2", dst );

    /*cvCvtColor(img_hsv,dst,CV_HSV2BGR);   
    cvErode(dst,dst,NULL,1);   
    cvDilate(dst,dst,NULL,1); 
    cvReleaseImage(&img_hsv); */
    //cvNamedWindow("skin",1);
    //cvShowImage("skin",img);
    //Release temp data
    //detect_and_draw(img);   
    for(i=0;i<img->height;i++)   
    {   
        delete(bmpdata[i]);
        delete(bmpdata1[i]);     
    }
    
    //cvShowImage( "dst", dst );
}   

void detect_and_draw( IplImage* img )
{

    // Create memory for calculations
    static CvMemStorage* storage = 0;

    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    int scale = 1;

    // Create a new image based on the input image
    IplImage* temp = cvCreateImage( cvSize(img->width/scale,img->height/scale), 8, 3 );

    // Create two points to represent the face locations
    CvPoint pt1, pt2;
    int i;

    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return;
    }
    
    // Allocate the memory storage
    storage = cvCreateMemStorage(0);

    // Create a new named window with title: result
    cvNamedWindow( "result", 1 );

    // Clear the memory storage which was used before
    cvClearMemStorage( storage );

    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {

        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(10, 10) );

        // Loop the number of faces found.
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
           // Create a new rectangle for drawing the face
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

            // Find the dimensions of the face,and scale it if necessary
            pt1.x = r->x*scale;
            pt2.x = (r->x+r->width)*scale;
            pt1.y = r->y*scale;
            pt2.y = (r->y+r->height)*scale;

            // Draw the rectangle in the input image
            cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
        }
    }

    // Show the image in the window named "result"
    cvShowImage( "result", img );

    // Release the temp image created.
    cvReleaseImage( &temp );
}
