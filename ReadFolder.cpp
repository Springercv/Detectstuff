/* Done by some sources
Reading folder:
http://www.linuxquestions.org/questions/programming-9/c-list-files-in-directory-379323/
Checking and passing "dead lock" by sign of root/up DIR returning: 
http://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder
*/
#include <time.h>  
#include <cv.h>   
#include <cxcore.h>   
#include <highgui.h>   

#include <sys/types.h>
#include "dirent.h"
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
const char* cascade_name =
    "C:/INI/Subversion/OpenCV/data/haarcascades/haarcascade_frontalface_alt.xml";    
string loct = "D:/lexuanpr/Datasets/FRGC/VJfaces";

void detect_and_draw( IplImage* img, string line);

int getdir (string dir, vector<string> &files)
{
  DIR *dp;
  struct dirent *dirp;
  string subdir;
  int nfiles = 0;
  //struct stat filestat;
  if((dp = opendir(dir.c_str())) == NULL) {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return errno;
  }
  while ((dirp = readdir(dp)) != NULL && dirp->d_name!="" && dirp->d_name!=".") {    
    if (dirp->d_type != DT_DIR){// && dirp->d_name!="" && dirp->d_namlen != 0){
      if ( strstr( dirp->d_name, ".JPG") || strstr( dirp->d_name, ".jpg") || strstr(dirp->d_name, ".ppm")){                 
          subdir = string(dir+ "/" + dirp->d_name);
          files.push_back(subdir);
          IplImage *imageBGR = cvLoadImage(subdir.c_str(), CV_LOAD_IMAGE_COLOR); 
          //cvShowImage("Input Image",imageBGR);
          nfiles++; 
          cout <<"Load file number: "<<nfiles <<endl << subdir << endl;
          detect_and_draw(imageBGR, string(dirp->d_name)); 
          
          //cvWaitKey();
       }
    }    

    else if (dirp->d_name!="" && dirp->d_namlen != 0 ){  
         if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
        		continue;
          subdir = dir + "/" + dirp->d_name;
          getdir(subdir.c_str(),files); 
          cout<<"This is opening directory "<<subdir<<endl;
        }      
  }
  closedir(dp);
  return 0;
}
void detect_and_draw( IplImage* img, string line)
{
    // Create memory for calculations
    cvErode(img, img,NULL,1);   
    cvDilate(img, img,NULL,1); 
    
    line = loct + "/" + line; 

    static CvMemStorage* storage = 0;

    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    int scale = 1;

    // Create a new image based on the input image
    IplImage* temp = cvCreateImage( cvSize(img->width/scale,img->height/scale), 8, 3 );

    // Create two points to represent the face locations
    CvPoint pt1, pt2;
    int i;
    
    stringstream sfile;// indexing the result image
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
            cvSetImageROI(img, *r);               
            //sprintf(filename1, "%s%d%s", base, i,filename); cout <<"Save to file: \n" << filename1<< endl;         

            //cvResize(small_img, face, CV_INTER_CUBIC);
            
            sfile <<i;                               
            line.insert(line.length()- 4,sfile.str());
            cout<<"save as: "<<line<<endl;                            
            cvSaveImage(line.c_str(), img);                
            cvResetImageROI(img);  
        }
        cvClearSeq(faces);
    }

    // Show the image in the window named "result"
    //cvShowImage( "result", img );

    // Release the temp image created.
    cvReleaseImage( &temp );
    cvReleaseImage( &img );
    cvReleaseHaarClassifierCascade( &cascade );
    cvReleaseMemStorage(&storage);  
 }
void MakeList ( vector<string> &files)
{
  ofstream myfile;
  string filename = "???.txt";
  //cout<< "Enter a file to open (leave blank to quit):\n";
  //getline( cin, filename );
  myfile.open( filename.c_str() );

  /*if ( dir == "" ) {
     exit(1);
  }*/

  for (unsigned int i = 0;i < files.size();i++) {
    if (myfile.is_open())
      {
        //cout << files[i] << endl; 
        myfile << files[i]<< endl;    
      }
      else cout << "Unable to open file";      
  }
}
void ProcessList ()
{
  string line;
  vector<string> files = vector<string>();
  ifstream myfile ("???.txt");//D:/lexuanpr/Datasets/Source_Image/file.lst*/
  if (myfile.is_open()) {
    while ( myfile.good() )
    {  
      getline (myfile,line); 
      getdir(line,files);
      //IplImage *imageBGR = cvLoadImage(line.c_str(), CV_LOAD_IMAGE_COLOR);    
      //detect_and_draw(imageBGR, line);      
    }
  }
  myfile.close();
 }
int main()
{
string dir = "D:/lexuanpr/Datasets/FRGC/nd1/Spring2003";
//cout<< "Enter a directory path to open (leave blank to quit):\n";
//getline( cin, dir );

//string dir = string(".");
vector<string> files = vector<string>();
getdir(dir,files);
  
return 0;
}
