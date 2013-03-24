//http://8a52labs.wordpress.com/category/opencv/

//OpenCV Headers

#include<cv.h>
#include<highgui.h>
//Input-Output
#include<stdio.h>
//Blob Library Headers
#include<cvblob.h>
//Definitions
#define h 240
#define w 320
//NameSpaces
using namespace cvb;
using namespace std;
int main()
{
//Structure to get feed from CAM
CvCapture* capture=cvCreateCameraCapture(0);
//Structure to hold blobs
CvBlobs blobs;
//Windows
cvNamedWindow("Live",CV_WINDOW_AUTOSIZE);
//Image Variables
IplImage *frame=cvCreateImage(cvSize(w,h),8,3);   //Original Image
IplImage *hsvframe=cvCreateImage(cvSize(w,h),8,3);//Image in HSV color space
IplImage *labelImg=cvCreateImage(cvSize(w,h),IPL_DEPTH_LABEL,1);//Image Variable for blobs
IplImage *threshy=cvCreateImage(cvSize(w,h),8,1); //Threshold image of yellow color
 
//Getting the screen information
int screenx = GetSystemMetrics(SM_CXSCREEN);
int screeny = GetSystemMetrics(SM_CYSCREEN);
 
while(1)
{
//Getting the current frame
IplImage *fram=cvQueryFrame(capture);
//If failed to get break the loop
if(!fram)
break;
//Resizing the capture
cvResize(fram,frame,CV_INTER_LINEAR );
//Flipping the frame
cvFlip(frame,frame,1);
//Changing the color space
cvCvtColor(frame,hsvframe,CV_BGR2HSV);
//Thresholding the frame for yellow
cvInRangeS(hsvframe,cvScalar(23,41,133),cvScalar(40,150,255),threshy);
//Filtering the frame
cvSmooth(threshy,threshy,CV_MEDIAN,7,7);
//Finding the blobs
unsigned int result=cvLabel(threshy,labelImg,blobs);
//Rendering the blobs
cvRenderBlobs(labelImg,blobs,frame,frame);
//Filtering the blobs
cvFilterByArea(blobs,60,500);
for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
{
double moment10 = it->second->m10;
double moment01 = it->second->m01;
double area = it->second->area;
//Variable for holding position
int x1;
int y1;
//Calculating the current position
x1 = moment10/area;
y1 = moment01/area;
//Mapping to the screen coordinates
int x=(int)(x1*screenx/w);
int y=(int)(y1*screeny/h);
//Printing the position information
cout<<"X: "<<x<<" Y: "<<y<<endl;
}
//Showing the images
cvShowImage("Live",frame);
//Escape Sequence
char c=cvWaitKey(33);
if(c==27)
break;
}
//Cleanup
cvReleaseCapture(&capture);
cvDestroyAllWindows();
 
}
