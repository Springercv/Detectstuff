//http://opencv-srf.blogspot.de/2010/09/object-detection-using-color-seperation.html
#include "stdafx.h"

#include <cv.h>
#include <highgui.h>
#include <iostream>  


//This function threshold the HSV image and create a binary image
IplImage* GetThresholdedImage(IplImage* imgHSV){       
IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
cvInRangeS(imgHSV, cvScalar(0, 58, 89), cvScalar(25, 173, 229), imgThresh);
return imgThresh;
}  //inRange(hsv, Scalar(0, 58, 89), Scalar(25, 173, 229), bw);
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
   

    /*Old version of ordinary GW
    //Inverse of the Avg values of the R,G,B
    mR = 1/(byter/height*width);
    mG = 1/(byteg/height*width);
    mB = 1/(byteb/height*width);
    //Smallest Avg Value (MAX because we are dealing with the inverses)
    maxRGB = max(max(mR, mG), mB);cout<<maxRGB<< endl;
    minRGB = min(min(mR, mG), mB);
    
    //Calculate the scaling factors
    mR = mR/maxRGB; 
    mG = mG/maxRGB; 
    mB = mB/maxRGB; */

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



int main(){
      
      CvCapture* capture =0;       
      capture = cvCaptureFromCAM(0);
      IplImage* frame=0;
      cvNamedWindow("Source");     
      cvNamedWindow("Result");

      if(!capture){
            printf("Capture failure, loading still image\n");
            frame = cvLoadImage("D:/lexuanpr/Datasets/Caltech/image_0003.jpg");            
            //Balance(frame);         
          
            frame=cvCloneImage(frame); 
            cvSmooth(frame, frame, CV_GAUSSIAN,3,3); //smooth the original image using Gaussian kernel
             //converting the original image into grayscale
            IplImage* imgGrayScale = cvCreateImage(cvGetSize(frame), 8, 1); 
            cvCvtColor(frame,imgGrayScale,CV_BGR2GRAY);
            IplImage* imgHSV = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
            cvCvtColor(frame, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
            IplImage* imgThresh = GetThresholdedImage(imgHSV);
          
            cvSmooth(imgThresh, imgThresh, CV_GAUSSIAN,3,3); //smooth the binary image using Gaussian kernel
            
            cvShowImage("Source", imgThresh);           
            cvShowImage("Result", frame);
            //thresholding the grayscale image to get better results
            cvThreshold(imgGrayScale,imgGrayScale,128,255,CV_THRESH_BINARY);  
           
            CvSeq* contours;  //hold the pointer to a contour in the memory block
            CvSeq* result;   //hold sequence of points of a contour
            CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours
           
            //finding all contours in the image
            cvFindContours(imgGrayScale, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
            
            //iterating through each contour
            while(contours)
            {
              result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);


            }
            cvWaitKey(0);

            //Clean up used images
            cvReleaseImage(&imgHSV);
            cvReleaseImage(&imgThresh);            
            cvReleaseImage(&frame);
        }

      cvDestroyAllWindows() ;
      cvReleaseCapture(&capture);     

      return 0;
}
