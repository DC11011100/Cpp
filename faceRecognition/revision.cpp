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
#define PATH_SAMPLE_DIR "training_data"
#define DEBUG

using namespace cv;
using namespace cv::ml;
using namespace std;

typedef enum
{
    s_Setup,
    s_Sample,
    s_Detect,
    s_Select,
    s_Exit
} State_t;

static State_t state = s_Setup;


// Uses HAAR cascades to detect faces. Also returns the Grayscale convervted frame
void filter_Faces(const Mat &frame, const Mat &bw_eq_frame, CascadeClassifier &face_cascade, vector<Rect> &faces)
{
  // Detect faces
  face_cascade.detectMultiScale(bw_eq_frame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
   
  // Draw faces
  for (int i = 0; i < faces.size(); i++)
  {
      Point top_left(faces[i].x, faces[i].y );
      Point bottom_right(faces[i].x + faces[i].width, faces[i].y + faces[i].height );
      
      // Make the first discovered face the Face of Interest
      if(i == 0) rectangle(frame, top_left, bottom_right, Scalar(0,255, 0), 4, 8, 0 );
      else        rectangle(frame, top_left, bottom_right, Scalar(255,0, 0), 4, 8, 0 );
  }
}

// File operations
uint8_t load_sample_count(int &positive, int &negative, const string &sample_dir_path)
{
    ifstream counts(sample_dir_path + "/info.txt");
    if(counts.good())
    {
        counts >> positive >> negative;
    } else
    {
        positive = 0;
        negative = 0;
    }
    
    counts.close();
    return 0;
}

uint8_t write_sample_count(int positive, int negative, const string &sample_dir_path)
{
    ofstream counts(sample_dir_path + "/info.txt");
    if(counts.good())
    {
        counts << "Number of p-samples = " << positive << endl
               << "Number of n-samples = " << negative << endl;
    } else
    {
        cout << "ERROR: Couldn't open " << sample_dir_path << endl;
    }

    counts.close();
    return 0;
}

// True for positive
uint8_t sample(bool sample_type, int sample_number, const string &sample_dir_path)
{
    if(sample_type)
    {
        cout << "Positive " << sample_number << endl;
    } else
    {
        cout << "Negative " << sample_number << endl;
    }
    return 0;
}

// State transitions
State_t parse_Keys(char key, char last_key, State_t current_state)
{
    // Check for requested state change
    if (last_key == '\e') 
    {
        switch(tolower(key))
        {
            case 's':
            {
                cout << "Sampling..." << endl
                     << "    " << "Press 'p' for a positive sample and 'n' for a negative sample" << endl;
                return s_Sample;
            }
            case 'd':
            {
                cout << "Training..." << endl;
                // Compute HOG feature vector
                
                // Write/cache  sample diagnostics
                
                // Train the SVM

                // Start using SVM to verify face
                cout << "Detecting..." << endl;
                return s_Detect;
            }
            case 'x':
            {
                cout << "Exiting..." << endl;
                return s_Exit;
            }

        }
    }
    
    // Instructions or stay in the same state
    if(key == '\e')
    {
        cout << "Menu:" << endl
             << "'s' --> Sample" << endl
             << "'d' --> Detect" << endl
             << "'x' --> Exit"   << endl;
    }
    
    return current_state;
}



int main(int argc, char** argv)
{
    // Open up camera
    VideoCapture video(0);
    if(!video.isOpened())
    {
        cout << "Unable to startup camera...Exiting!\n";
        return -1;
    }
    
    // Load up face detection cascades for face localization HAAR
    cout << "Loading HAAR data..." << endl;
    CascadeClassifier face_cascade;
    if( !face_cascade.load( PATH_FACE_CASCADE ) )
    { 
        printf("--(!)Error loading\n"); 
        return -1; 
    }
    
    // Load sample diagnostics
    cout << "Loading HOG data" << endl;
    int positive_count, negative_count;
    load_sample_count(positive_count, negative_count, PATH_SAMPLE_DIR);
    cout << "Loaded:" << endl
         << "p-count = " << positive_count << endl
         << "n-count = " << negative_count << endl;

    char key = '\0';
    char last_key = '\0';

    // Routine
    for(;;)
    {
        // Capture frame
        Mat frame;
        video >> frame;
        if(frame.empty()) continue; // Skip bad frames

        // Parse keyboard input
        key = waitKey(30);
        state    = parse_Keys(key, last_key, state);
        if(key > 0)
        {
            last_key = key;
        }

        // Get gray scale image and Histogram Equalize to diminish lighting influences
        Mat bw_frame, bw_eq_frame;
        cvtColor(frame, bw_frame, CV_BGR2GRAY);
        equalizeHist(bw_frame, bw_eq_frame); // Potential Improvement: https://stackoverflow.com/questions/15007304/histogram-equalization-not-working-on-color-image-opencv

        // Detect faces and draw onto frame
        vector<Rect> faces(16);
        filter_Faces(frame, bw_eq_frame, face_cascade, faces);             
        imshow("Filter Cascades", frame);

        switch(state)
        {
            case(s_Setup):
            {   
                cout << "Press 'esc' for the Menu" << endl
                     << "Let's start by sampling." << endl
                     << "Press 'p' for a positive sample and 'n' for a negative sample" << endl
                     << "Note: Have roughly equal number of positive/negative samples to mitigate biasing" << endl;
                state = s_Sample;
                break;
            }
            case(s_Sample):
            {
                switch(key)
                {
                    case 'p':
                    {
                        cout << "+1 Positive Sample" << endl;
                        sample(true, positive_count++, PATH_SAMPLE_DIR);
                        
                        break;
                    }
                    case 'n':
                    {
                        cout << "+1 Negative Sample" << endl;
                        sample(false, negative_count++, PATH_SAMPLE_DIR);

                        break;
                    }
                }
                break;
            }
            case(s_Detect):
            {
                break;
            }
            case(s_Select):
            {
                break;
            }
            case(s_Exit):
            {
                // Save sample diagnostics
                write_sample_count(positive_count, negative_count, PATH_SAMPLE_DIR);

                // Save SVM XML

                return 0;
            }
            default:
            {
                cout << __FILE__ << ":" << __func__ << ":" << "Line: " << __LINE__
                     << "Corrupted state...Exiting!" << endl;
                return 1;
            }
        }
    }
}
