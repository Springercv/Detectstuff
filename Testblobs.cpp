// Copyright (C) 2007 by Cristóbal Carnero Liñán
// grendel.ccl@gmail.com
//
// This file is part of cvBlob.
//
// cvBlob is free software: you can redistribute it and/or modify
// it under the terms of the Lesser GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cvBlob is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Lesser GNU General Public License for more details.
//
// You should have received a copy of the Lesser GNU General Public License
// along with cvBlob.  If not, see <http://www.gnu.org/licenses/>.
//

#include <iostream>
//#include <iomanip>
using namespace std;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__) || (defined(__APPLE__) & defined(__MACH__)))
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <cvblob.h>
using namespace cvb;

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

int main()
{
  int distance; 
  CvPoint pt1, pt2;

  IplImage *img = cvLoadImage("D:/lexuanpr/Datasets/Caltech/Skin/image_0219.jpg");//"test.png");//, 1);//SkinBin/Add/All/
  //cvErode(img, img,NULL,1);   
  //cvDilate(img, img,NULL,1);
  //cvSetImageROI(img, cvRect(100, 100, 800, 500));

  IplImage *grey = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
  IplImage *dst = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);

  cvCvtColor(img, grey, CV_BGR2GRAY);

  cvThreshold(grey, grey, 100, 255, CV_THRESH_BINARY);
  
  IplImage *labelImg = cvCreateImage(cvGetSize(grey),IPL_DEPTH_LABEL,1);
  
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* comp = NULL;
  cvPyrSegmentation( grey, dst, storage, &comp, 4, 200, 50 );
  int n_comp = comp->total;
  for( int i=0; i<n_comp; i++ ) {
    CvConnectedComp* cc = (CvConnectedComp*) cvGetSeqElem( comp, i );
    pt1.x = cc->rect.x; 
    pt2.x = cc->rect.x + cc->rect.width;
    pt1.y = cc->rect.y;
    pt2.y = cc->rect.y + cc->rect.height;
    cout<<"comp number:  "<<i<<endl;
    cout<<pt1.x<<" "<<pt1.y<<" "<<pt2.x<<" "<<pt2.y<<endl;
    cvRectangle( img, pt1, pt2, CV_RGB(255,255,0), 3, 8, 0 );            
  }

  cvNamedWindow("ConnectedComp", 1);
  cvShowImage("ConnectedComp", img);
  cvReleaseMemStorage( &storage );
  

  /*CvBlobs blobs; 
  unsigned int result = cvLabel(grey, labelImg, blobs);

  IplImage *imgOut = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3); cvZero(imgOut);
  cvRenderBlobs(labelImg, blobs, img, imgOut);
  //cvFilterByArea(blobs, 1, 1000);
  cout<<cvLargestBlob(blobs); 
  //unsigned int i = 0;
  CvRect crect = cvRect(0,0,0,0); 
  CvBlobs::const_iterator it=blobs.begin();
  crect.x= int((*it).second->minx);
  crect.y = int((*it).second->miny);
  crect.width = int((*it).second->maxx)-int((*it).second->minx);
  crect.height = int((*it).second->maxy)-int((*it).second->miny);
  // Render contours:
  for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
  {
    //cvRenderBlob(labelImg, (*it).second, img, imgOut);
    
    CvScalar meanColor = cvBlobMeanColor((*it).second, labelImg, img);
    distance = (*it).second->minx - (*it).second->minx;    
    //cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );            

    crect.x= int((*it).second->minx);
    crect.y = int((*it).second->miny);
    crect.width = int((*it).second->maxx)-int((*it).second->minx);
    crect.height = int((*it).second->maxy)-int((*it).second->miny);
    
    
    //cout << "Mean color: r=" << (unsigned int)meanColor.val[0] << ", g=" << (unsigned int)meanColor.val[1] << ", b=" << (unsigned int)meanColor.val[2] << endl;
    
    CvContourPolygon *polygon = cvConvertChainCodesToPolygon(&(*it).second->contour);

    CvContourPolygon *sPolygon = cvSimplifyPolygon(polygon, 200.);
    CvContourPolygon *cPolygon = cvPolygonContourConvexHull(sPolygon);

    cvRenderContourChainCode(&(*it).second->contour, imgOut);
    cvRenderContourPolygon(sPolygon, imgOut, CV_RGB(0, 0, 255));
    cvRenderContourPolygon(cPolygon, imgOut, CV_RGB(0, 255, 0));

    delete cPolygon;
    delete sPolygon;
    delete polygon;
    
    // Render internal contours:
    for (CvContoursChainCode::const_iterator jt=(*it).second->internalContours.begin(); jt!=(*it).second->internalContours.end(); ++jt)
      cvRenderContourChainCode((*jt), imgOut);

    //stringstream filename;
    //filename << "blob_" << setw(2) << setfill('0') << i++ << ".png";
    //cvSaveImageBlob(filename.str().c_str(), imgOut, (*it).second);
  }

  cvNamedWindow("test", 1);
  cvShowImage("test", imgOut);
  //cvShowImage("grey", grey);
  cvWaitKey(0);
  cvDestroyWindow("test");

  cvReleaseImage(&imgOut);
  cvReleaseImage(&grey);
  cvReleaseImage(&labelImg);
  cvReleaseImage(&img);

  cvReleaseBlobs(blobs);*/
  cvWaitKey(0);
  return 0;
}
