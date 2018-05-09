#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <typeinfo>
#define PATH_FACE_CASCADE "./HAAR_faceData/haarcascade_frontalface_alt.xml"
#define PATH_SAMPLE_DIR "training_data"
#define DEBUG

using namespace cv;
using namespace cv::ml;
using namespace std;



class FaceIdentifier
{
    private:
        vector  <Mat>  current_batch;
        HOGDescriptor hog;
        vector  <Mat>  gradients;
        vector <int>  labels;
        vector <float> descriptors;
        vector <Point> locations;
        Ptr<SVM> svm;

        int positive_count;
        int negative_count; 
        
        /*
        * Convert training/testing set to be used by OpenCV Machine Learning algorithms.
        * TrainData is a matrix of size (#samples x max(#cols,#rows) per samples), in 32FC1.
        * Transposition of samples are made if needed.
        */
        // TODO
        void forceFit(const vector <Mat> & training_set_partition)
        {
            printf("Enforcing compatability for batch of %d", training_set_partition.size());
            //--Convert data
            const int rows = (int)training_set_partition.size();
            const int cols = (int)std::max( training_set_partition[0].cols, training_set_partition[0].rows );
            
            Mat tmp(1, cols, CV_32FC1); //< used for transposition if needed
            current_batch = Mat(rows, cols, CV_32FC1);
            vector <Mat>::const_iterator itr = training_set_partition.begin();
            vector <Mat>::const_iterator end = training_set_partition.end();
            for( int i = 0 ; itr != end ; ++itr, ++i )
            {
                CV_Assert( itr->cols == 1 ||
                    itr->rows == 1 );
                if( itr->cols == 1 )
                {
                    transpose( *(itr), tmp );
                    tmp.copyTo( current_batch.row( i ) );
                }
                else if( itr->rows == 1 )
                {
                    itr->copyTo( current_batch.row( i ) );
                }
            }
        } 
        void initSVM()
        { 
            Ptr <SVM> svm = SVM::create();
            svm->setCoef0(0.0);
            svm->setDegree(3);
            svm->setTermCriteria(TermCriteria( CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1000, 1e-3 ));
            svm->setGamma(0);
            svm->setKernel(SVM::LINEAR);
            svm->setNu(0.5);
            svm->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
            svm->setC(0.01); // From paper, soft classifier
            svm->setType(SVM::EPS_SVR); // C_SVC; // EPSILON_SVR; // may be also NU_SVR; // do regression task
        }

        void writeSample(const Mat & src, const string & filename, bool isPositive)
        {
            // Write image
            imwrite(filename, src);
        }

        
        void push_HOG(vector <Mat> & gradients, Mat & roi, HOGDescriptor & hog, vector <Point> & location, vector <float> & descriptors) 
        {
            // Resize for uniform comparison
            resize(roi, roi, Size(128,128) );
            hog.winSize = roi.size();

            // Compute HOG data and append to gradient list
            hog.compute( roi, descriptors, Size(32,32), Size(0,0), location );
            gradients.push_back( Mat(descriptors).clone() );
        }

    public:
        FaceIdentifier() 
        {
            initSVM();
            positive_count = 0;
            negative_count = 0;
        }
        void load(const string & path_savefile) 
        {
            svm->load(path_savefile);
        }

        void sample(bool isPositive, Mat & cropped_face)
        {
            push_HOG(gradients, cropped_face, hog, locations, descriptors);
            labels.push_back(isPositive ? +1 : -1);
            char savename[64] = {0};
            sprintf(savename, "./training_HOGs/%s.%d.png", isPositive ? "pos" : "neg", 
                            isPositive ? positive_count : negative_count); 
            imwrite(savename, cropped_face);
            positive_count++;
            negative_count++;
        }

        void train(const vector <Mat> & hogs, const vector <int> & labels)
        {
            cout << "Traing batch";
            
            // Make sure data can be mapped to classifier dimensions, set as current batch and train
            forceFit(hogs);
            svm->train(current_batch, ROW_SAMPLE, Mat(labels));
        }

        void save(const string & path_savefile)
        {
            svm->save(path_savefile);
        }


class HaarClassifier
{
    private:
        Mat mcurrent_frame_conditioned;
        vector<Rect> vfaces_in_frame; // Size 16
        CascadeClassifier cascade; 
        
        void neatralizeInput(const Mat &finput, Mat &foutput)
        {
              // Get gray scale image and Histogram Equalize to diminish lighting influences
              Mat inter;
              cvtColor(finput, inter, CV_BGR2GRAY);
              // Potential Improvement:
              // https://stackoverflow.com/questions/15007304/histogram-equalization-not-working-on-color-image-opencv
              equalizeHist(inter, foutput);
        }



    // Uses HAAR cascades to detect faces. Also returns the Grayscale convervted frame
    void drawFaces(const Mat &display)
    {
        // Draw faces onto display frame
        for (int i = 0; i < vfaces_in_frame.size(); i++)
        {
            Point top_left(vfaces_in_frame[i].x, vfaces_in_frame[i].y );
            Point bottom_right(vfaces_in_frame[i].x + vfaces_in_frame[i].width, vfaces_in_frame[i].y + vfaces_in_frame[i].height );
            
            // Make the first discovered face the Face of Interest
            if(i == 0) rectangle(display, top_left, bottom_right, Scalar(0,255, 0), 4, 8, 0 );
            else       rectangle(display, top_left, bottom_right, Scalar(255,0, 0), 4, 8, 0 );
        }
    }

    // Desc : Update faces vector with faces in given frame 
    void findFaces(const Mat &finput)
    {
        cascade.detectMultiScale(mcurrent_frame_conditioned, 
                                      vfaces_in_frame, 
                                      1.1, 2, 
                                      0|CV_HAAR_SCALE_IMAGE, 
                                      Size(30, 30));
    }

    public:

        void showFaces(const Mat &finput)
        {
            // Read current frame, dont bother processing bad frames
            if (finput.empty()) return;

            // Massage the input a bit, and store for reference
            neatralizeInput(finput, mcurrent_frame_conditioned);
      
            Mat display;
            // Update faces vector with those in current frame then overlay onto display
            findFaces(finput);
            drawFaces(display);
            
            imshow("Filter Cascades", display);
            
        }

        bool load(const string &path_cascades_yml)
        {
            printf("Loading HAAR data from\n    '%s'", path_cascades_yml);
            if( !cascade.load(path_cascades_yml) )
            { 
                printf("ERROR: Couldn't load HAAR coefficients file"); 
                return false; 
            }
            else return true;
        }
};






typedef enum
{
    s_Setup,
    s_Sample,
    s_Detect,
    s_Select,
    s_Exit
} State_t;

const string getStateName(State_t state)
{
    switch(state)
    {
        case s_Setup  : return "Setup";
        case s_Sample : return "Sampling";
        case s_Detect : return "Detecting";
        case s_Select : return "Selecting";
        case s_Exit   : return "Exiting";
    }
}

static State_t state = s_Setup;


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

/*
* Convert training/testing set to be used by OpenCV Machine Learning algorithms.
* TrainData is a matrix of size (#samples x max(#cols,#rows) per samples), in 32FC1.
* Transposition of samples are made if needed.
*/
void convert_to_ml(const std::vector< cv::Mat > & train_samples, cv::Mat& trainData )
{
    //--Convert data
    const int rows = (int)train_samples.size();
    const int cols = (int)std::max( train_samples[0].cols, train_samples[0].rows );
    cv::Mat tmp(1, cols, CV_32FC1); //< used for transposition if needed
    trainData = cv::Mat(rows, cols, CV_32FC1 );
    vector< Mat >::const_iterator itr = train_samples.begin();
    vector< Mat >::const_iterator end = train_samples.end();
    for( int i = 0 ; itr != end ; ++itr, ++i )
    {
        CV_Assert( itr->cols == 1 ||
            itr->rows == 1 );
        if( itr->cols == 1 )
        {
            transpose( *(itr), tmp );
            tmp.copyTo( trainData.row( i ) );
        }
        else if( itr->rows == 1 )
        {
            itr->copyTo( trainData.row( i ) );
        }
    }

}


// Example from OpenCV2 SDK
void train_svm( const vector< Mat > & gradient_lst, const vector< int > & labels )
{
    Mat train_data;
    convert_to_ml( gradient_lst, train_data );

    clog << "Start training...";
    Ptr<SVM> svm = SVM::create();
    svm->setCoef0(0.0);
    svm->setDegree(3);
    svm->setTermCriteria(TermCriteria( CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1000, 1e-3 ));
    svm->setGamma(0);
    svm->setKernel(SVM::LINEAR);
    svm->setNu(0.5);
    svm->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
    svm->setC(0.01); // From paper, soft classifier
    svm->setType(SVM::EPS_SVR); // C_SVC; // EPSILON_SVR; // may be also NU_SVR; // do regression task
    svm->train(train_data, ROW_SAMPLE, Mat(labels));
    clog << "...[done]" << endl;

    svm->save( "myFace.perceptron.yml" );
}


// State transitions
# define BTN_MENU 27 // Esc
State_t parse_Keys(char key, char last_key, State_t current_state)
{
    // Check for requested state change
    if (last_key == BTN_MENU) // Esc
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
    if(key == BTN_MENU)
    {
        cout << "Menu (" << getStateName(current_state) << "):" << endl
             << "'s' : Sample +/-" << endl
             << "'d' : Detect (Post-training)" << endl
             << "'x' : Exit and Save"   << endl
             << "'l' : Show positions of faces" << endl;
    }
    return current_state;
}


bool cameraCheck(VideoCapture stream)
{
    if (!stream.isOpened())
    {
        cout << "Unable to startup camera...Exiting!\n";
        return false;
    } else return true;
}

int main(int argc, char** argv)
{
    // Open up camera
    VideoCapture video(0);
    if(!cameraCheck(video)) return -1;
    

    // Load up face detection cascades for face localization HAAR
    HaarClassifier haar = HaarClassifier();
    if (!haar.load( PATH_FACE_CASCADE )) return -1;

    // Prepare Face Classifier
    FaceIdentifier detective = FaceIdentifier();
    
    // Load sample diagnostics
    cout << "Loading HOG data" << endl;
    int positive_count, negative_count;
    load_sample_count(positive_count, negative_count, PATH_SAMPLE_DIR);
    cout << "Loaded:" << endl
         << "p-count = " << positive_count << endl
         << "n-count = " << negative_count << endl;



    // Routine
    char key = '\0';
    char last_key = '\0';
    Mat current_frame;
    for(;;)
    {
        // Capture frame
        video >> current_frame;

        // Parse keyboard input
        key   = waitKey(30);
        state = parse_Keys(key, last_key, state);
        if(key > 0) last_key = key;

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
                    default:
                    {
                        haar.showFaces(current_frame);
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
