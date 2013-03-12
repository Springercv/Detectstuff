//Not yet check
///////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <cv.h>
#include <highgui.h>

int main()
{
     //load the video file to the memory
     CvCapture *capture =     cvCaptureFromAVI("C:/Wildlife.avi");

     if( !capture ) return 1;

     //obtain the frames per seconds of that video
     int fps = ( int )cvGetCaptureProperty( capture, CV_CAP_PROP_FPS );

    //create a window with the title "Video"
    cvNamedWindow("Video");

    while(true) {
             //grab and retrieve each frames of the video sequencially 
             IplImage* frame = cvQueryFrame( capture );

             if( !frame ) break;

             //show the retrieved frame in the "Video" window
             cvShowImage( "Video", frame );

             int c;


             if(fps!=0){ 

                     //wait for 1000/fps milliseconds
                     c = cvWaitKey(1000/fps);
            }else{
                     //wait for 40 milliseconds
                      c = cvWaitKey(40);
            }



          //exit the loop if user press "Esc" key  (ASCII value of "Esc" is 27) 
            if((char)c==27 ) break;
   }

   //destroy the opened window
   cvDestroyWindow("Video");   
   //release memory
   cvReleaseCapture( &capture );

    return 0;

}
///////////////////////////////////////////////////////////////////////////////////////
