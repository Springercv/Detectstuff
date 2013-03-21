//Try using cvinRange(hsv, Scalar(0, 58, 89), Scalar(25, 173, 229), bw); from
//http://bsd-noobz.com/opencv-guide/60-4-skin-detection


#ifdef _CH_
#pragma package <opencv>
#endif
#include "stdafx.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <ctime>

const char *VENTANA="Blob detection";

IplImage* imagen;
IplImage* imagen_color;
IplImage* smooth;
IplImage* threshold;
IplImage* open_morf;
IplImage* blobResult;
IplImage* img_contornos;
CvMoments moments;
CvHuMoments humoments;
CvSeq* contour;
CvSeq* contourLow;
void Contour();
struct num   
    {   
        unsigned char H;   
        unsigned char S;   
        unsigned char V;   
    };   

const char* cascade_name =
    "C:/INI/Subversion/OpenCV/data/haarcascades/haarcascade_frontalface_alt.xml";    

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
//This function threshold the HSV image and create a binary image

IplImage* GetThresholdedImage(IplImage* imgHSV){        
       IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
       cvInRangeS(imgHSV, cvScalar(0, 58, 89), cvScalar(25, 173, 229), imgThresh); 
       return imgThresh;
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
    //cvNamedWindow( "result", 1 );

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
    //cvShowImage( "result", img );

    // Release the temp image created.
    cvReleaseImage( &temp );
}
int main( int argc, char** argv )
{
  srand ( time(NULL) );
  /**
  * Init
  **/
  printf( "Blob detection and feature extraction\n"
  "\tESC - quit the program\n"
  "\tInspired by David Millan Escriva | Damiles\n");

  /**
  * Init images
  **/

  //Check if user specify image to process
  char* filename= "D:/lexuanpr/Datasets/Caltech/image_0003.jpg";

  //load image in gray level
  imagen=cvLoadImage(filename,0);
  //load image in RGB
  imagen_color=cvLoadImage(filename,1);
  

  //Create needed images
  smooth= cvCreateImage(cvSize(imagen->width, imagen->height), IPL_DEPTH_8U, 1);
  threshold= cvCreateImage(cvSize(imagen->width, imagen->height), IPL_DEPTH_8U, 1);
  open_morf= cvCreateImage(cvSize(imagen->width, imagen->height), IPL_DEPTH_8U, 1);

  //Init variables for countours
  contour = 0;
  contourLow = 0;
  //Create storage needed for contour detection
  CvMemStorage* storage = cvCreateMemStorage(0);

  //Create window
  cvNamedWindow( VENTANA, 0 );


  //Smooth image
  cvSmooth(imagen, smooth, CV_GAUSSIAN, 3, 0, 0, 0);

  CvScalar avg;
  CvScalar avgStd;
  cvAvgSdv(smooth, &avg, &avgStd, NULL);
  printf("Avg: %f\nStd: %f\n", avg.val[0], avgStd.val[0]);
  //threshold image
  cvThreshold(smooth, threshold, (int)avg.val[0]-7*(int)(avgStd.val[0]/8), 255, CV_THRESH_BINARY_INV);
  //Morfologic filters
  cvErode(threshold, open_morf, NULL,1);cvDilate(open_morf, open_morf, NULL,1);
  
  //Duplicate image for countour
  img_contornos=cvCloneImage(open_morf);

  //Search countours in preprocesed image
  cvFindContours( img_contornos, storage, &contour, sizeof(CvContour),CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0) );

  //Optimize contours, reduce points
  contourLow=cvApproxPoly(contour, sizeof(CvContour), storage,CV_POLY_APPROX_DP,1,1);
  
  double t = 0;
  t = (double)cvGetTickCount();

  Color(imagen_color);
  //For each contour found
  for( ; contourLow != 0; contourLow = contourLow->h_next )
  {
      CvScalar color = CV_RGB( rand()&200, rand()&200, rand()&200 );//define color to draw rectangle
      //We can draw the contour of object
      //cvDrawContours( imagen_color, contourLow, color, color, -1, 0, 8, cvPoint(0,0) );
      //Or detect bounding rect of contour
      CvRect rect;
      CvPoint pt1, pt2;
      rect=cvBoundingRect(contourLow, NULL);
      pt1.x = rect.x;
      pt2.x = (rect.x+rect.width);
      pt1.y = rect.y;
      pt2.y = (rect.y+rect.height);

      //cvRectangle(imagen_color, pt1,pt2, color, 1, 8, 0);
      t = (double)cvGetTickCount() - t;
      printf( "Blob detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );

      IplImage* imgHSV = cvCreateImage(cvGetSize(imagen_color), IPL_DEPTH_8U, 3); 
      cvCvtColor(imagen_color, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
      IplImage* imgThresh = GetThresholdedImage(imgHSV);
      cvSetImageROI(imagen_color,rect );          
      detect_and_draw(imagen); cvResetImageROI(imagen_color);

  }
  t = (double)cvGetTickCount() - t;
  printf( "face detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );

  cvShowImage(VENTANA, imagen_color);

  //Main Loop
  for(;;)
  {
  int c;

  c = cvWaitKey(10);
  if( (char) c == 27 )
  break;
  else if((char) c == '3')
  cvShowImage(VENTANA, threshold);
  else if((char) c=='1')
  cvShowImage(VENTANA, imagen);
  else if((char) c=='2')
  cvShowImage(VENTANA, smooth);
  else if((char) c=='4')
  cvShowImage(VENTANA, open_morf);
  else if((char) c=='r')
  cvShowImage(VENTANA, imagen_color);

  }

  cvDestroyWindow(VENTANA);

  return 0;
}
