#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

CascadeClassifier cascade;
vector<Rect> faces;
vector<Rect> windows;

/**
 * Return a new search window given a rectangle of detected face
 */
Rect getWindow(Rect r)
{
    Point pt(r.x + r.width/2, r.y + r.height/2);

    Rect win;
    win.width  = 120;
    win.height = 120;
    win.x      = pt.x - win.width/2;
    win.y      = pt.y - win.height/2;

    return win;
}

/**
 * Draw the search windows and detected faces
 */
void drawResult(Mat& frame)
{
    for (int i = 0; i < faces.size(); i++)
    {
        Rect r = faces[i];

        rectangle(
            frame, 
            Point(r.x, r.y), 
            Point(r.x + r.width, r.y + r.height), 
            CV_RGB(255,0,0)
        );
    }

    for (int i = 0; i < windows.size(); i++)
    {
        Rect r = windows[i];

        rectangle(
            frame, 
            Point(r.x, r.y), 
            Point(r.x + r.width, r.y + r.height), 
            CV_RGB(0,255,0)
        );
    }
}

/**
 * Detect faces using OpenCV's built-in module
 */
void detectInitialFaces(Mat& frame)
{
    Mat gray;
    cvtColor(frame, gray, CV_BGR2GRAY);
    equalizeHist(gray, gray);

    cascade.detectMultiScale(gray, faces, 1.2, 3);

    for (int i = 0; i < faces.size(); i++)
        windows.push_back(getWindow(faces[i]));
}

/**
 * Detect the largest blob in a binary image
 */
Rect detectLargestBlob(Mat& bw)
{
    rectangle(bw, Point(0,0), Point(bw.cols, bw.rows), Scalar(0));

    vector<vector<Point> > contours;
    findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    if (contours.size() == 0)
        return Rect(0, 0, bw.cols, bw.rows);

    drawContours(bw, contours, -1, Scalar(255), -1);
    findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    int maxArea = 0;
    int maxIdx = 0;

    for (int area = 0, i = 0; i < contours.size(); i++)
    {
        int area = (int)contourArea(contours[i]);
        maxIdx  = area > maxArea ? i : maxIdx;
        maxArea = area > maxArea ? area : maxArea;
    }

    return boundingRect(contours[maxIdx]);
}

/**
 * Detect faces with skin detection
 */
void processFrame(Mat& frame)
{
    for (int i = 0; i < windows.size(); i++)
    {
        Rect win = windows[i];

        win.x      = std::max(win.x, 0);
        win.y      = std::max(win.y, 0);
        win.width  = std::min(win.width, frame.cols - win.x);
        win.height = std::min(win.height, frame.rows - win.y);

        Mat subimg = frame(win);

        Mat hsv;
        cvtColor(subimg, hsv, CV_BGR2HSV);

        Mat bw;
        inRange(hsv, Scalar(0,51,89), Scalar(17,140,255), bw);

        faces[i] = detectLargestBlob(bw);
        faces[i].x += win.x;
        faces[i].y += win.y;

        windows[i] = getWindow(faces[i]);
    }
}

int main()
{
    if (!cascade.load("haarcascade_frontalface_alt.xml"))
        return -1;

    VideoCapture capture("jquery_conf.mpg");
    if (!capture.isOpened())
        return -1;

    bool first_frame = true;

    while(true)
    {
        Mat frame;
        capture >> frame;
        if (frame.empty())
            break;

        if (first_frame)
        {
            detectInitialFaces(frame);
            first_frame = false;
        }
        else
        {
            processFrame(frame);
        }

        drawResult(frame);
        imshow("video", frame);

        if (waitKey(20) == 'q')
            break;
    }

    return 0;
}
