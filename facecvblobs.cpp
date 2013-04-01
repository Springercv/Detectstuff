// ObjectTracking.cpp : Define the entry point for console app.  
//http://www.technical-recipes.com/2011/tracking-coloured-objects-in-video-using-opencv/
#include "cv.h"  
#include "highgui.h"  
#include "BlobResult.h"  
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
using namespace std;

IplImage* originalThr = 0;
IplImage* original = 0;

const char* cascade_name =
    "C:/OpenCV2.1/data/haarcascades/haarcascade_frontalface_alt.xml"; 

//detect face
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
		cvResetImageROI(img);	

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

// Get thresholded image in HSV format  
IplImage* GetThresholdedImageHSV( IplImage* img )  
{  
    // Create an HSV format image from image passed  
    IplImage* imgHSV = cvCreateImage( cvGetSize( img ), 8, 3 );     
    cvCvtColor( img, imgHSV, CV_BGR2HSV );  
  
    // Create binary thresholded image acc. to max/min HSV ranges  
    // For detecting blue gloves in "MOV.MPG - HSV mode  
    IplImage* imgThresh = cvCreateImage( cvGetSize( img ), 8,  1 );           
  
    cvInRangeS( imgHSV, cvScalar(0, 58, 89), cvScalar(25, 173, 229),  imgThresh );  
  
    // Tidy up and return thresholded image  
    cvReleaseImage( &imgHSV );  
    return imgThresh;  
}  
void detectblob(IplImage* frame){
		
    CBlobResult blobs;    
    CBlob *currentBlob;   
    CvPoint pt1, pt2;  
    CvRect cvRect;  
    //int key = 0;  
    //IplImage* frame = 0;   
    // Windows used to display input video with bounding rectangles  
    // and the thresholded video  
    cvNamedWindow( "video" );  
    cvNamedWindow( "thresh" );        

		if(!originalThr)
		{
			original = cvCreateImage( cvGetSize(frame), 8, 1);		
			originalThr = cvCreateImage(cvGetSize(original), IPL_DEPTH_8U,1);
		}
        // Get object's thresholded image (blue = white, rest = black)  
		IplImage* imgThresh = GetThresholdedImageHSV( frame );
		cvThreshold( original, originalThr, 100, 255, CV_THRESH_BINARY );
        

        // Detect the white blobs from the black background  
        blobs = CBlobResult( originalThr, NULL, 0 );    
  
        // Exclude white blobs smaller than the given value (10)    
        // The bigger the last parameter, the bigger the blobs need    
        // to be for inclusion    
        blobs.Filter( blobs,  B_EXCLUDE, CBlobGetArea(),  B_LESS, 100);           
  
        // Attach a bounding rectangle for each blob discovered  
		int num_blobs = blobs.GetNumBlobs();  
		
		double t = 0;
		t = (double)cvGetTickCount();

        for ( int i = 0; i < num_blobs; i++ )    
        {                 	
            currentBlob = blobs.GetBlob( i );               
            cvRect = currentBlob->GetBoundingBox();  
			
			cvSetImageROI(imgThresh, cvRect ); 
			if  (cvCountNonZero(imgThresh) > 0){
	 		  //  detect(frame, cvRect);  
				}
 	        cvResetImageROI(imgThresh);
			
			pt1.x = cvRect.x;  pt1.y = cvRect.y;  pt2.x = cvRect.x + cvRect.width; 
			pt2.y = cvRect.y + cvRect.height;  
			// Attach bounding rect to blob in orginal video input  
            cvRectangle( frame, pt1,   pt2, cvScalar(255, 255, 0, 0),  1, 8,   0 );  
        }  
		t = (double)cvGetTickCount() - t;
	    printf( " Processing time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );

        // Add the black and white and original images  
        cvShowImage( "thresh", imgThresh );  
        cvShowImage( "video", frame );  
  
        // Optional - used to slow up the display of frames  
        //key = cvWaitKey( 2000 / fps );  
  
        // Prevent memory leaks by releasing thresholded image  
        cvReleaseImage( &imgThresh );    
	    cvReleaseImage( &original );
		cvReleaseImage( &originalThr );
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
    cvShowImage( "result", img );

    // Release the temp image created.
    cvReleaseImage( &temp );
	cvReleaseMemStorage(&storage);
}
int main()  
{ 
   string line;
  line=line.c_str();
  ifstream myfile ("Caltech.txt");//D:/lexuanpr/Datasets/Source_Image/file.lst
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {  
      getline (myfile,line);      
      IplImage *imageBGR = cvLoadImage(line.c_str(), CV_LOAD_IMAGE_COLOR);    
      /*cvShowImage("Input Image", imageBGR);
      Balance(imageBGR);
      cvShowImage("Balanced Image", imageBGR);
      SkinColor(imageBGR);
      cvShowImage("Skin Image", imageBGR);*/		

	 //detect_and_draw(imageBGR);
     detectblob(imageBGR);	

      cout << line << endl;
      cvWaitKey();
    }
    myfile.close();
  }

  else cout << "Unable to open file";
  /*CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
   if ( !capture ) {
     fprintf( stderr, "ERROR: capture is NULL \n" );
     getchar();
     return -1;
   }
   while ( 1 ) {
     // Get one frame
     IplImage* frame = cvQueryFrame( capture );
     if ( !frame ) {
       fprintf( stderr, "ERROR: frame is null...\n" );
       getchar();
       break;
     }
     detectblob(frame);
     if ( (cvWaitKey(10) & 255) == 27 ) break;
   }
   // Release the capture device housekeeping
   cvReleaseCapture( &capture );
   cvDestroyWindow( "mywindow" );	*/ 
  return 0;  
}  
