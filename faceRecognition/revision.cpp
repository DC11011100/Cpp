#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <time.h>

#define PATH_FACE_CASCADE "./HAAR_faceData/haarcascade_frontalface_alt.xml"
#define DEBUG

using namespace cv;
using namespace cv::ml;
using namespace std;

typedef enum
{
    s_Setup,
    s_Sample,
    s_Search,
    s_Done
} State_t;

static State_t state = s_Setup;


// Uses HAAR cascades to detect faces. Also returns the Grayscale convervted frame
vector <Rect> detectAndDisplay(Mat frame, Mat frame_gray, CascadeClassifier face_cascade )
{
  std::vector<Rect> faces;

  //-- Detect faces
  face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
  
  // [FIX ME]: Only single-face capture when taking P-samples
  /*
  if ( faces.size() > 1) 
  {
      printf("Only one face at time!\n");
      return vector<Rect>();
  }*/ 
  
  // Display square around all faces 
  for (int i=0; i<faces.size(); i++)
  {
      Point TL_corner( faces[i].x, faces[i].y );
      Point TR_corner( faces[i].x + faces[i].width, faces[i].y + faces[i].height );
      if( i == 0) rectangle( frame, TL_corner, TR_corner, Scalar(0,255, 0), 4, 8, 0 );
      else        rectangle( frame, TL_corner, TR_corner, Scalar(255,0, 0), 4, 8, 0 );
  }
  
  return faces;
}



int main(int argc, char** argv)
{
    // Shared data

    VideoCapture video(0);
    if(!video.isOpened())
    {
        cout << "Unable to startup camera...Exiting!\n";
        return -1;
    }
        
    // Routine
    for(;;)
    {
        char kin = waitKey(30);
        if(kin == 'q') state = s_Done;

        Mat frame;
        video >> frame;
        if(frame.empty()) continue; // Skip bad frames

        // Get gray scale image and Histogram Equalize to diminish lighting influences
        Mat bw_sample;
        cvtColor( frame, bw_sample, CV_BGR2GRAY );
        equalizeHist( bw_sample, bw_sample);
      

        // Load up face detection cascades for face localization HAAR
        CascadeClassifier face_cascade;
        vector<Rect> faces(10);
        if( !face_cascade.load( PATH_FACE_CASCADE ) )
        { 
            printf("--(!)Error loading\n"); 
            return -1; 
        }
        faces = detectAndDisplay( frame, bw_sample, face_cascade);        

        imshow("Filter Cascades", frame);

        switch(state)
        {
            case(s_Setup):
            {
                state = s_Sample;
                break;
            }
            case(s_Sample):
            {
                break;
            }
            case(s_Search):
            {
                break;
            }
            case(s_Done):
            {
                cout << "Thanks for trying me out...Exiting!\n";
                return 0;
            }
            default:
            {
                cout << "Oh no! Something unexpected has happened\n"
                     << "ERROR: Your state has gone haywire!\n";
            }
        }
    }
}
