// lkdemo.cpp : Defines the entry point for the console application.

#include "stdafx.h"

/* Demo of modified Lucas-Kanade optical flow algorithm.
   See the printf below */

#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <ctype.h>

IplImage *image = 0, *hsv_img = 0, *grey = 0, *prev_grey = 0, *pyramid = 0, *prev_pyramid = 0, *swap_temp;

int win_size = 10;
const int MAX_COUNT = 500;
CvPoint2D32f* points[2] = {0,0}, *swap_points;
char* status = 0;
int counter = 0;
int need_to_init = 1;
int night_mode = 0;
int flags = 0;
int add_remove_pt = 0;
CvPoint pt;
double r, g, b = 0;

//Definition of Color elements
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
    "C:/INI/Subversion/OpenCV/data/haarcascades/haarcascade_frontalface_alt.xml"; 

void on_mouse( int event, int x, int y, int flags, void* param )
{
    if( !image )
        return;

    if( image->origin )
        y = image->height - y;

    if( event == CV_EVENT_LBUTTONDOWN )
    {
        pt = cvPoint(x,y);
        add_remove_pt = 1;
    }
}


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
    //cout<<"size:"<<height*width<< endl; 
    np = height*width - np; //cout<<"non-black pixels:"<<np<< endl;   // Number of non-black pixels
    mR = 1/(byter/np);//cout<<"R-average:"<<mR<< endl;
    mG = 1/(byteg/np);//cout<<"G-average:"<<mG<< endl;
    mB = 1/(byteb/np);//cout<<"B-average:"<<mB<< endl;
    
    //Cstd = 2*np/Cstd; cout<<"Cstd:"<<Cstd<< endl;
    Cstd = max(max(mR, mG), mB); //cout<<Cstd<< endl;    

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
IplImage* convertRGBtoHSV(const IplImage *imageRGB)  // From Shevin to fix problem of low intensity in OpenCV
{
  float fR, fG, fB;
	float fH, fS, fV;
	const float FLOAT_TO_BYTE = 255.0f;
	const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;

	// Create a blank HSV image
	IplImage *imageHSV = cvCreateImage(cvGetSize(imageRGB), 8, 3);
	if (!imageHSV || imageRGB->depth != 8 || imageRGB->nChannels != 3) {
		printf("ERROR in convertImageRGBtoHSV()! Bad input image.\n");
		exit(1);
	}

	int h = imageRGB->height;		// Pixel height.
	int w = imageRGB->width;		// Pixel width.
	int rowSizeRGB = imageRGB->widthStep;	// Size of row in bytes, including extra padding.
	char *imRGB = imageRGB->imageData;	// Pointer to the start of the image pixels.
	int rowSizeHSV = imageHSV->widthStep;	// Size of row in bytes, including extra padding.
	char *imHSV = imageHSV->imageData;	// Pointer to the start of the image pixels.
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			// Get the RGB pixel components. NOTE that OpenCV stores RGB pixels in B,G,R order.
			uchar *pRGB = (uchar*)(imRGB + y*rowSizeRGB + x*3);
			int bB = *(uchar*)(pRGB+0);	// Blue component
			int bG = *(uchar*)(pRGB+1);	// Green component
			int bR = *(uchar*)(pRGB+2);	// Red component

			// Convert from 8-bit integers to floats.
			fR = bR * BYTE_TO_FLOAT;
			fG = bG * BYTE_TO_FLOAT;
			fB = bB * BYTE_TO_FLOAT;

			// Convert from RGB to HSV, using float ranges 0.0 to 1.0.
			float fDelta;
			float fMin, fMax;
			int iMax;
			// Get the min and max, but use integer comparisons for slight speedup.
			if (bB < bG) {
				if (bB < bR) {
					fMin = fB;
					if (bR > bG) {
						iMax = bR;
						fMax = fR;
					}
					else {
						iMax = bG;
						fMax = fG;
					}
				}
				else {
					fMin = fR;
					fMax = fG;
					iMax = bG;
				}
			}
			else {
				if (bG < bR) {
					fMin = fG;
					if (bB > bR) {
						fMax = fB;
						iMax = bB;
					}
					else {
						fMax = fR;
						iMax = bR;
					}
				}
				else {
					fMin = fR;
					fMax = fB;
					iMax = bB;
				}
			}
			fDelta = fMax - fMin;
			fV = fMax;				// Value (Brightness).
			if (iMax != 0) {			// Make sure its not pure black.
				fS = fDelta / fMax;		// Saturation.
				float ANGLE_TO_UNIT = 1.0f / (6.0f * fDelta);	// Make the Hues between 0.0 to 1.0 instead of 6.0
				if (iMax == bR) {		// between yellow and magenta.
					fH = (fG - fB) * ANGLE_TO_UNIT;
				}
				else if (iMax == bG) {		// between cyan and yellow.
					fH = (2.0f/6.0f) + ( fB - fR ) * ANGLE_TO_UNIT;
				}
				else {				// between magenta and cyan.
					fH = (4.0f/6.0f) + ( fR - fG ) * ANGLE_TO_UNIT;
				}
				// Wrap outlier Hues around the circle.
				if (fH < 0.0f)
					fH += 1.0f;
				if (fH >= 1.0f)
					fH -= 1.0f;
			}
			else {
				// color is pure Black.
				fS = 0;
				fH = 0;	// undefined hue
			}

			// Convert from floats to 8-bit integers.
			int bH = (int)(0.5f + fH * 255.0f);
			int bS = (int)(0.5f + fS * 255.0f);
			int bV = (int)(0.5f + fV * 255.0f);

			// Clip the values to make sure it fits within the 8bits.
			if (bH > 255)
				bH = 255;
			if (bH < 0)
				bH = 0;
			if (bS > 255)
				bS = 255;
			if (bS < 0)
				bS = 0;
			if (bV > 255)
				bV = 255;
			if (bV < 0)
				bV = 0;

			// Set the HSV pixel components.
			uchar *pHSV = (uchar*)(imHSV + y*rowSizeHSV + x*3);
			*(pHSV+0) = bH;		// H component
			*(pHSV+1) = bS;		// S component
			*(pHSV+2) = bV;		// V component
		}
	}
	return imageHSV;
}
void detect_and_draw( IplImage* img )
{

    // Create memory for calculations
    static CvMemStorage* storage = 0;

    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    int scale = 2;
    char filename1[50]="D:/lexuanpr/Photos/Test/";

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
        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        
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

            //cout << "Images from file: \n" <<filename << endl;
            //cvSetImageROI(small_img, rect);               
          //  sprintf(filename1, "%s%d%s", base, i,filename); cout <<"Save to file: \n" << filename1<< endl;         

            //cvResize(small_img, face, CV_INTER_CUBIC);
            //cvSaveImage(filename1, face);
            //cvResetImageROI(small_img);           
            // Draw the rectangle in the input image
            cvRectangle( small_img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );            
            
        }        
        cvClearSeq(faces); 
    }
    
    // Show the image in the window named "result"
    cvShowImage( "result", small_img );

    // Release the temp image created.
    cvReleaseImage( &face );
    cvReleaseImage( &img ); 
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );
    //cvClearMemStorage(  storage );
    cvReleaseMemStorage( &storage );
    
    //free(filename);
    free(filename1);
}

void RGBtoHSV( double r, double g, double b, double *h, double *s, double *v )
{/*
	double max = (r > g && r > b)? r : (g > b)? g : b;
	double min = (r < g && r < b)? r : (g < b)? g : b;
	double delta = max-min;
	*v = max/255;
	*s = delta/255;
	if(max == r)
		*h = (g - b) / delta;
	else if(max == g)
		*h = 1.0/3 + (b - r) / delta;
	else
		*h = 2.0/3 + (r - g) / delta;
	if (*h < 0)
		*h += 1;
*/

	double delta;
	double max = (r > g && r > b)? r : (g > b)? g : b;
	double min = (r < g && r < b)? r : (g < b)? g : b;
	cout<<"r = "<<r<<endl;
	cout<<"g = "<<g<<endl;
	cout<<"b = "<<b<<endl;	


	*v = max / 255.0;				// v

	delta = max - min;

	if( max != 0 )
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}

	if( r == max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan

	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;
	
	//cout<<"h = "<<*h<<endl;
	//cout<<"s = "<<*s<<endl;
	//cout<<"v = "<<*v<<endl;
}
void ColorCheck( float r, float g, float b, int color)
{
	float min, max, delta;
	float h, s, v;
	r = r/255; cout<<"r="<<r<<endl; 
	g = g/255; cout<<"g="<<g<<endl;
	b = b/255; cout<<"b="<<b<<endl;  
	min = MIN( MIN(r, g), b );
	max = MAX( MAX(r, g), b );
	v = max;				// v

	delta = max - min;

	if( max != 0 )
		s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		s = 0;
		h = -1;
		//return;
	}

	if( r == max )
		h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		h = 4 + ( r - g ) / delta;	// between magenta & cyan

	h = 60;				// degrees
	if( h < 0 )
		h += 360;
  
  //Detect skin-like pixel 
  if(h<=12&&s>=54)   //if(bmpdata[i][j].H<=19&&bmpdata[i][j].S>=48)   
       color =255;        
  else    color=h=s=v=0;   
  cout<<"h = "<<h<<endl;
  cout<<"s = "<<s<<endl;
  cout<<"v = "<<v<<endl;
}
void convertRGBtoHSV(const IplImage *imageRGB, int x, int y, int c)
{
	float fR, fG, fB;
	float fH, fS, fV;
	const float FLOAT_TO_BYTE = 255.0f;
	const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;

      char *imRGB = imageRGB->imageData;	// Pointer to the start of the image pixels.
      int rowSizeRGB = imageRGB->widthStep;	// Size of row in bytes, including extra padding.

	
			// Get the RGB pixel components. NOTE that OpenCV stores RGB pixels in B,G,R order.
			uchar *pRGB = (uchar*)(imRGB + y*rowSizeRGB + x*3);
			int bB = *(uchar*)(pRGB+0);	// Blue component
			int bG = *(uchar*)(pRGB+1);	// Green component
			int bR = *(uchar*)(pRGB+2);	// Red component

			// Convert from 8-bit integers to floats.
			fR = bR * BYTE_TO_FLOAT;
			fG = bG * BYTE_TO_FLOAT;
			fB = bB * BYTE_TO_FLOAT;

			// Convert from RGB to HSV, using float ranges 0.0 to 1.0.
			float fDelta;
			float fMin, fMax;
			int iMax;
			// Get the min and max, but use integer comparisons for slight speedup.
			if (bB < bG) {
				if (bB < bR) {
					fMin = fB;
					if (bR > bG) {
						iMax = bR;
						fMax = fR;
					}
					else {
						iMax = bG;
						fMax = fG;
					}
				}
				else {
					fMin = fR;
					fMax = fG;
					iMax = bG;
				}
			}
			else {
				if (bG < bR) {
					fMin = fG;
					if (bB > bR) {
						fMax = fB;
						iMax = bB;
					}
					else {
						fMax = fR;
						iMax = bR;
					}
				}
				else {
					fMin = fR;
					fMax = fB;
					iMax = bB;
				}
			}
			fDelta = fMax - fMin;
			fV = fMax;				// Value (Brightness).
			if (iMax != 0) {			// Make sure its not pure black.
				fS = fDelta / fMax;		// Saturation.
				float ANGLE_TO_UNIT = 1.0f / (6.0f * fDelta);	// Make the Hues between 0.0 to 1.0 instead of 6.0
				if (iMax == bR) {		// between yellow and magenta.
					fH = (fG - fB) * ANGLE_TO_UNIT;
				}
				else if (iMax == bG) {		// between cyan and yellow.
					fH = (2.0f/6.0f) + ( fB - fR ) * ANGLE_TO_UNIT;
				}
				else {				// between magenta and cyan.
					fH = (4.0f/6.0f) + ( fR - fG ) * ANGLE_TO_UNIT;
				}
				// Wrap outlier Hues around the circle.
				if (fH < 0.0f)
					fH += 1.0f;
				if (fH >= 1.0f)
					fH -= 1.0f;
			}
			else {
				// color is pure Black.
				fS = 0;
				fH = 0;	// undefined hue
			}

			// Convert from floats to 8-bit integers.
			int bH = (int)(0.5f + fH * 255.0f);
			int bS = (int)(0.5f + fS * 255.0f);
			int bV = (int)(0.5f + fV * 255.0f);

			// Clip the values to make sure it fits within the 8bits.
			if (bH > 255)		bH = 255;			if (bH < 0)			bH = 0;
			if (bS > 255)		bS = 255;			if (bS < 0)			bS = 0;
			if (bV > 255)		bV = 255;			if (bV < 0)			bV = 0;

			// Set the HSV pixel components.
      cout<<"H="<<bH<<"  ";// H component
      cout<<"S="<<bS<<"  ";// S component
      cout<<"V="<<bV<<endl;// V component

      if(bH>=92&&bS>=54)					//if(bmpdata[i][j].H<=19&&bmpdata[i][j].S>=48)   
					{
 					c =255; 
					}
				else c=0;				
							
			cout<<"c="<<c<<endl<<endl;
	
	//return c;
}
int main( int argc, char** argv )
{
    CvCapture* capture = 0;

    if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
        capture = cvCaptureFromCAM( argc == 2 ? argv[1][0] - '0' : 0 );
    else if( argc == 2 )
        capture = cvCaptureFromAVI( argv[1] );

    if( !capture )
    {
     //   fprintf(stderr,"Could not initialize capturing...\n");
        //return -1;
    }

    /* print a welcome message, and the OpenCV version */
    printf ("Welcome to lkdemo, using OpenCV version %s (%d.%d.%d)\n",
	    CV_VERSION,
	    CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);

    printf( "Hot keys: \n"
            "\tESC - quit the program\n"
            "\tr - auto-initialize tracking\n"
            "\tc - delete all the points\n"
            "\tn - switch the \"night\" mode on/off\n"
            "To add/remove a feature point click it\n" );

    cvNamedWindow( "LkDemo", 0 );
    cvSetMouseCallback( "LkDemo", on_mouse, 0 );
     
    int c, i, j, k, height, width;
	  double h,s,v;

    for(;;)
    {
        IplImage* frame = 0;
       	//char *filename = "D:/lexuanpr/Datasets/Caltech/image_0003.jpg";	// Use a default image file.
       	//IplImage *frame = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
        // New code above

        frame = cvQueryFrame( capture );
        if( !frame )        
        break;
        
        if( !image )
        {
            /* allocate all the buffers */
            image = cvCreateImage( cvGetSize(frame), 8, 3 );
			      image->origin = frame->origin;
			      hsv_img= cvCreateImage( cvGetSize(frame), 8, 3 );
            grey = cvCreateImage( cvGetSize(frame), 8, 1 );
            prev_grey = cvCreateImage( cvGetSize(frame), 8, 1 );            
            pyramid = cvCreateImage( cvGetSize(frame), 8, 1 );
            prev_pyramid = cvCreateImage( cvGetSize(frame), 8, 1 );
            points[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
            points[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
            status = (char*)cvAlloc(MAX_COUNT);
            flags = 0;
        }
        
        cvCopy( frame, image, 0 );
		    cvCopy( image, hsv_img, 0 );
        cvCvtColor( image, grey, CV_BGR2GRAY );
     	  cvCvtColor(image,hsv_img, CV_BGR2HSV);

        /* setup the pointer to access image data */
        int Onchannels = image->nChannels;
        int Ostep      = image->widthStep;
        uchar *Odata = ( uchar* )image->imageData;  
		
		    int nchannels = hsv_img->nChannels;
        int step      = hsv_img->widthStep;
        uchar *data = ( uchar* )hsv_img->imageData;  

        if( night_mode )
            cvZero( image );
        
        if( need_to_init )
        {
            /* automatic initialization */
            IplImage* eig = cvCreateImage( cvGetSize(grey), 32, 1 );
            IplImage* temp = cvCreateImage( cvGetSize(grey), 32, 1 );
            double quality = 0.01;
            double min_distance = 10;

            counter = MAX_COUNT;
            cvGoodFeaturesToTrack( grey, eig, temp, points[1], &counter,
                                   quality, min_distance, 0, 3, 0, 0.04 );
            cvFindCornerSubPix( grey, points[1], counter,
                cvSize(win_size,win_size), cvSize(-1,-1),
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
            cvReleaseImage( &eig );
            cvReleaseImage( &temp );

            add_remove_pt = 0;
        }
        
        else if( counter > 0 )
        {
            cvCalcOpticalFlowPyrLK( prev_grey, grey, prev_pyramid, pyramid,
                points[0], points[1], counter, cvSize(win_size,win_size), 3, status, 0,
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), flags );
            flags |= CV_LKFLOW_PYR_A_READY;
            for( i = k = 0; i < counter; i++ )
            {
                if( add_remove_pt )
                {
                    double dx = pt.x - points[1][i].x;
                    double dy = pt.y - points[1][i].y;

                    if( dx*dx + dy*dy <= 25 )
                    {
                        add_remove_pt = 0;
                        continue;
                    }
                }

                if( !status[i] )
                    continue;

                points[1][k++] = points[1][i]; 
				height = cvPointFrom32f(points[1][i]).y; 
				width = cvPointFrom32f(points[1][i]).x;
				cout<<"height i="<<height<<"    "<<"width i="<<width<<endl;

        convertRGBtoHSV(image,width, height, c); 
        
				cvCircle (image, cvPointFrom32f(points[1][i]), 10, CV_RGB(0,c,0), -1, 8,0); 
				
				//SkinColor(image);
				// print hsv values of first pixel  
                //ColorCheck(r, g, b, c); RGBtoHSV(r,g,b,&h,&s,&v);
				
                //cvCircle( image, cvPointFrom32f(points[1][i]), 3, CV_RGB(0,255,0), -1, 8,0); //draw points
            }
            
            counter = k;
            //SkinColor(image);
            
        }//endif

        if( add_remove_pt && counter < MAX_COUNT )
        {
            points[1][counter++] = cvPointTo32f(pt);
            cvFindCornerSubPix( grey, points[1] + counter - 1, 1,
                cvSize(win_size,win_size), cvSize(-1,-1),
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
            add_remove_pt = 0;
        }

        CV_SWAP( prev_grey, grey, swap_temp );
        CV_SWAP( prev_pyramid, pyramid, swap_temp );
        CV_SWAP( points[0], points[1], swap_points );
        need_to_init = 0;
        cvShowImage( "LkDemo", image );

        c = cvWaitKey(10);
        if( (char)c == 27 )
            return -1;
            //break;
        switch( (char) c )
        {
        case 'r':
            need_to_init = 1;
            break;
        case 'c':
            counter = 0;
            break;
        case 'n':
            night_mode ^= 1;
            break;
        default:
            ;
        }
    }//end for

    cvReleaseCapture( &capture );
    cvDestroyWindow("LkDemo");
    cvWaitKey(0);
    //return 0;
}


