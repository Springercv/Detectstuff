//Try using cvinRange(hsv, Scalar(0, 58, 89), Scalar(25, 173, 229), bw); from
//http://bsd-noobz.com/opencv-guide/60-4-skin-detection


#ifdef _CH_
#pragma package <opencv>
#endif

#include "stdafx.h"
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <stdio.h>
#include <time.h>

struct num   
    {   
        unsigned char H;   
        unsigned char S;   
        unsigned char V;   
    };   
struct num1   
    {   
        unsigned char R;   
        unsigned char G;   
        unsigned char B;   
    };  
struct num2   
    {   
        unsigned char meanR;   
        unsigned char meanG;   
        unsigned char meanB;   
    };  
const char* cascade_name =
    "C:/OpenCV2.1/data/haarcascades/haarcascade_frontalface_alt.xml"; 

void Balance(IplImage *img) //Start with GW 
{   
    /* retrieve properties */
    int width     = img->width;
    int height    = img->height;
    int nchannels = img->nChannels;
    int step      = img->widthStep;
    float maxval = 0, maxRGB = 0;
     
    /* setup the pointer to access image data */
    uchar *data = ( uchar* )img->imageData;    
     
    /* Initializing values */
    int i, j, r, g, b;
    float byter = 0, byteg = 0, byteb = 0, stdR = 0, stdG = 0, byteB = 0, //sum of every channel
    mR = 0, mG = 0, mB = 0, np = 0, Cstd =0, minRGB =0;

    //Accessing data value and sum up into byte,
    //n* stand for the sum of black pixels
    for( i = 0 ; i < height ; i++ ) {
        for( j = 0 ; j < width ; j++ ) {
  
          if (((data[i*step + j*nchannels + 0]==0)&&(data[i*step + j*nchannels + 1]==0))&&(data[i*step + j*nchannels + 2]==0)){np++; //Counting the number of black pixels
          }
          else        {
            Cstd = max(max(data[i*step + j*nchannels + 0], data[i*step + j*nchannels + 1]), data[i*step + j*nchannels + 2])+ 
            min(min(data[i*step + j*nchannels + 0], data[i*step + j*nchannels + 1]), data[i*step + j*nchannels + 2])+ Cstd; //Counting the sum of max+ min Value to add on Cstd
            byter = data[i*step + j*nchannels + 0] + byter;//Sum of the values of R channel by every pixel                  
            byteg = data[i*step + j*nchannels + 1] + byteg;          
            byteb = data[i*step + j*nchannels + 2] + byteb;           
          }
        }
    }
   


    //New version to solve over compensation problem
    cout<<"size:"<<height*width<< endl; 
    np = height*width - np; cout<<"non-black pixels:"<<np<< endl;   // Number of non-black pixels
    mR = 1/(byter/np);cout<<"R-average:"<<mR<< endl;
    mG = 1/(byteg/np);cout<<"G-average:"<<mG<< endl;
    mB = 1/(byteb/np);cout<<"B-average:"<<mB<< endl;
    
    //Cstd = 2*np/Cstd; cout<<"Cstd:"<<Cstd<< endl;
    Cstd = max(max(mR, mG), mB); cout<<Cstd<< endl;    

    //New version to solve over compensation problem
    mR = mR/Cstd; 
    mG = mG/Cstd; 
    mB = mB/Cstd;
    
    //Rendering color using scaling factors
    for( i = 0 ; i < height ; i++ ) {
        for( j = 0 ; j < width ; j++ ) {                            
                     
            data[i*step + j*nchannels + 0] = (int)data[i*step + j*nchannels + 0] * mR;
            data[i*step + j*nchannels + 1] = (int)data[i*step + j*nchannels + 1] * mG;
            data[i*step + j*nchannels + 2] = (int)data[i*step + j*nchannels + 2] * mB;
        }
    }
}
void detect_and_draw( IplImage* img, CvRect r )
{

    // Create memory for calculations
    static CvMemStorage* storage = 0;

    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    int scale = 2;

    // Create a new image based on the input image
    //IplImage* temp = cvCreateImage( cvSize(img->width/scale,img->height/scale), 8, 3 );

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
    
    //
    IplImage* gray = cvCreateImage(cvSize( cvRound(img->width/scale), cvRound(img->height/scale)), 8, 1);
    IplImage* small_img = cvCreateImage(cvSize( cvRound(img->width/scale), cvRound(img->height/scale)), 8, 3);
    cvResize( img, small_img, CV_INTER_LINEAR );
    cvCvtColor( small_img, gray, CV_BGR2GRAY );
    cvEqualizeHist( gray, gray );
    IplImage* face = cvCreateImage(cvSize( 140, 140), 8, 3);            

    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {    
	    cvSetImageROI(img, r); 

        CvSeq* faces = cvHaarDetectObjects( gray, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40, 40) );

        // Loop the number of faces found.
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
           // Create a new rectangle for drawing the face
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

            // Find the dimensions of the face,and scale it if necessary
            pt1.x = r->x;
            pt2.x = r->x+r->width;
            pt1.y = r->y;
            pt2.y = r->y+r->height;
            
            CvRect rect = {r->x, r->y, r->width, r->height};

            cvRectangle( small_img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );            
            
        }        
        cvClearSeq(faces); 
    }
    
    // Release the temp image created.
    cvReleaseImage( &face );
    cvReleaseImage( &img ); 
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );
    //cvClearMemStorage(  storage );
    cvReleaseMemStorage( &storage );
    
    //free(filename);
    //free(filename1);
}


void SkinColor(IplImage *img)   
{       
    unsigned char meanH = 0;   
    unsigned char meanS = 0;   
    unsigned char meanV = 0;
    unsigned maxval;
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
   
    for(i=0;i<img->height;i++)     //Access image address and read data using widthStep from OpenCV
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
            if(bmpdata[i][j].H<=12&&bmpdata[i][j].S>=54)   //if(bmpdata[i][j].H<=19&&bmpdata[i][j].S>=48)   
            bmpdata[i][j].H+=0;   
            else    bmpdata[i][j].H=bmpdata[i][j].S=bmpdata[i][j].V=0;   
          }   
    }       
   
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

   
    for(i=0;i<img->height;i++)   
    {   
        delete(bmpdata[i]);
        delete(bmpdata1[i]);     
    }
    
    //cvShowImage( "dst", dst );
}   
IplImage* GetThresholdedImage(IplImage* imgHSV){        
       IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
       cvInRangeS(imgHSV, cvScalar(0, 58, 89), cvScalar(25, 173, 229), imgThresh); 
       return imgThresh;
} 
void detect( IplImage* img, CvRect r )
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

		cvSetImageROI(img, r); 

        CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40, 40) );

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
const char *VENTANA="Blob detection";

IplImage* imagen, *imagen_color, * smooth, * threshold;
IplImage* open_morf, * blobResult,* img_contornos;
CvMoments moments;
CvHuMoments humoments;
CvSeq* contour;
CvSeq* contourLow;
void Contour();
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
  char* filename= "F:/Users/Xuan/Documents/Datasets/Caltech/image_0003.jpg";

  //load image in gray level
  imagen=cvLoadImage(filename,0);  //cvShowImage("simple", imagen);

  //load image in RGB
  imagen_color=cvLoadImage(filename,1); // cvShowImage("color", imagen_color);

  

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
  cvErode(threshold, open_morf, NULL,1);
  cvDilate(open_morf, open_morf, NULL,1);
  //Duplicate image for countour
  img_contornos=cvCloneImage(open_morf);
	double t = 0;
	t = (double)cvGetTickCount();

  //Search countours in preprocesed image
  cvFindContours( img_contornos, storage, &contour, sizeof(CvContour),
  CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0) );
  //Optimize contours, reduce points
  contourLow=cvApproxPoly(contour, sizeof(CvContour), storage,CV_POLY_APPROX_DP,1,1);

  //Show to user the 7 hu moments (features detected) we can use this for clasification
  //printf("hu1\t\thu2\t\thu3\t\thu4\t\thu5\t\thu6\t\thu7\n");
  //For each contour found
  IplImage* imgHSV = cvCreateImage(cvGetSize(imagen_color), IPL_DEPTH_8U, 3); 
  cvCvtColor(imagen_color, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
  IplImage* imgThresh = GetThresholdedImage(imgHSV);

  for( ; contourLow != 0; contourLow = contourLow->h_next )
  {
	  CvScalar color = CV_RGB( rand()&200, rand()&200, rand()&200 );
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
	  cvRectangle(imagen_color, pt1,pt2, color, 1, 8, 0);
	  
	  cvSetImageROI(imgThresh,rect ); 
	  cout<<cvCountNonZero(imgThresh);
	  if  (cvCountNonZero(imgThresh) > 0){
		  detect(imagen_color, rect);  		  
		  //cvWaitKey(0);
		  }
	   cvResetImageROI(imagen_color);	
       cvResetImageROI(imgThresh);	
  }
	cvShowImage("imgThresh", imgThresh);
	cvShowImage(VENTANA, imagen_color);			  		  
  	t = (double)cvGetTickCount() - t;
    printf( "Blob detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );
	
  

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

  //cvDestroyWindow(VENTANA);

  return 0;
}
