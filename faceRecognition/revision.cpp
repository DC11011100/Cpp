// THE paper: https://lear.inrialpes.fr/people/triggs/pubs/Dalal-cvpr05.pdf
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
#define FILENAME_SESSION_DATA "info.txt"
#define PATH_SAMPLE_DIR "training_data"

#define PATH_SAVE_MODEL "myFace.yml"

#define DEBUG
#define DEBUG_STEP
//#define DEBUG_SHOW_SAMPLE

#define FACE_INPUT_WINDOW Size(128, 128)
#define HOG_STRIDE_WINDOW Size(8, 8)
#define HOG_STRIDE_PADDING Size(0, 0)


typedef enum
{
    s_Setup,
    s_Sample,
    s_Detect,
    s_Select,
    s_Exit
} State_t;

static State_t state = s_Setup;

using namespace cv;
using namespace cv::ml;
using namespace std;

Mat get_hogdescriptor_visu(const Mat& color_origImg, vector<float>& descriptorValues, const Size & size );

void error(string funcname, string msg)
{
    cout << " [" << funcname << "] " << "ERROR: " << msg << endl;
}

void warning(string funcname, string msg)
{
    cout << " [" << funcname << "] " << "WARNING: " << msg << endl;
} 

class FaceIdentifier
{
    private:
        struct Batch
        {
            vector < Mat > hog_descriptors;  // 
            vector < int > labels;     // +1, -1. Corresponds with the nth sample

            bool push( bool sample_type, const Mat & foi )
            {
                if (foi.empty()) return false;

                HOGDescriptor hog;
                vector <float> descriptors;
                Mat regularized_foi; 
               
                // Resize input faces to same size to maintain constant dimensionality of input layer
                resize(foi, regularized_foi, FACE_INPUT_WINDOW);
                hog.winSize = regularized_foi.size();
                hog.compute( regularized_foi, descriptors, HOG_STRIDE_WINDOW, HOG_STRIDE_PADDING );

                // Associate the computed HOG with a +/- value label for SVM
                hog_descriptors.push_back( Mat(descriptors).clone() );
                labels.push_back(sample_type ? +1 : -1);

#ifdef DEBUG
                imshow("HOG Visual", get_hogdescriptor_visu(regularized_foi, descriptors, regularized_foi.size() ));
#endif
                return true;
            }

            int size() 
            {
                CV_Assert(hog_descriptors.size() == labels.size());
                return labels.size();
            }

            void dump()
            {
                cout << "Dumping HOG Descriptors for batch" << endl;
                for (int i = 0; i < hog_descriptors.size(); i++)
                {
                    Mat hog = hog_descriptors[i];
                    string sign = labels[i] > 0 ? "+" : "-";
                    if (labels[i] == 0) sign = "0";
                    cout << "(" << sign << ")" << " Sample #" << i 
                         << " -- " << hog.rows << "x" << hog.cols;
                    
                    // Dump the descriptor. It will only have one row, but a big one! So, just average 
                    // and print that as some distinctive metric
                    double sum = 0;
                    for (int j = 0; j < hog.rows; j++)
                    {
                        sum += hog.at<float>(i,0);
                    }
                    cout << " -- " << "Avg:" << sum << endl;
                }
            }
        };

        // Flattening a gradient grid gives a single sample's Input layer vector. We'll need our SVM to 
        // to correlate this with one our binary labels. 
        // In my case (Is the face on screen David)? : yes-label : no-label
        vector <int>  labels;

        // Once we train an SVM, we can load a HOG Descriptor with it and it'll take care of computing and 
        // evaluating incoming hogs
        HOGDescriptor * hog_detector;

        // Same things as gradients but left 
        Ptr    <SVM> svm;
        bool     trained;
        Batch batch;

        // Session dir is for storing the post-conditioned sample images
        string session_dir;
        string session_metadata;

        int positive_count;
        int negative_count; 
        
        // TODO
        // Desc: Images get WxH for HOG descriptor which corresponds to subtiles for HOG
        //       This subtile matrix results in a matrix of gradients also WxH
        //       Func flattens this WxH for our 1-D input layer. It does this for all samples in batch
        //       creating a matrix which is First_layer_width x Nsamples, with each row being a sample's
        //       flattened HOG
        
        //TODO: fix up usages of this function 
        void compatBatch(Mat & batch_inputL1, const vector <Mat> & hog_descriptors)
        {
            int rows = hog_descriptors.size();
            int cols = std::max( hog_descriptors[0].cols, hog_descriptors[0].rows );
            
            Mat tmp(1, cols, CV_32FC1); // < used for transposition if needed
            batch_inputL1 = Mat(rows, cols, CV_32FC1);
            
            vector <Mat>::const_iterator itr = hog_descriptors.begin();
            for( int i = 0 ; i < hog_descriptors.size() ; i++ )
            {
                const Mat & d = hog_descriptors[i];
                CV_Assert( d.cols == 1 || d.rows == 1 );
                if( d.cols == 1 ) {
                    transpose( d , tmp );
                    tmp.copyTo( batch_inputL1.row(i) );
                } else if( d.rows == 1 )
                {
                    d.copyTo( batch_inputL1.row(i) );
                }
            }
        } 
        void initSVM()
        { 
            svm = SVM::create();
            svm->setCoef0(0.0);
            svm->setDegree(3);
            svm->setTermCriteria(TermCriteria( TermCriteria::MAX_ITER + TermCriteria::EPS, 1000, 1e-3 ));
            svm->setGamma(0);
            svm->setKernel(SVM::LINEAR);
            svm->setNu(0.5);
            svm->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
            svm->setC(0.01); // From paper, soft classifier
            svm->setType(SVM::EPS_SVR); // C_SVC; // EPSILON_SVR; // may be also NU_SVR; // do regression task
        }
         
        // TODO: Verify still working
        // Desc: From the sample directory, loads metadata about session like number of samples etc.
        uint8_t loadSession()
        {
            ifstream meta(session_metadata);
            if(meta.good())
            {
                meta >> positive_count >> negative_count;
            } else
            {
                positive_count = 0;
                negative_count = 0;
            }
            
            meta.close();
            return 0;
        }
        

    public:
        FaceIdentifier(const string & sample_dir, const string & meta_filename) : 
            session_dir(sample_dir),
            session_metadata(sample_dir + "/" + meta_filename)

        {
            positive_count = 0;
            negative_count = 0;
            trained        = false;
        }


        // TODO: Verify still working
        uint8_t saveSession()
        {
            ofstream meta(session_metadata);
            if(meta.good())
            {
                meta << "Number of p-samples = " << positive_count << endl
                       << "Number of n-samples = " << negative_count << endl;
            } else
            {
                cout << "ERROR: Couldn't open " << session_metadata << endl;
            }

            meta.close();
            return 0;
        }

        // TODO: Merge this with loadSession()
        void loadPretrained(const string & path_savefile) 
        {
            // Metadata for corresponding session
            

            // Load previously trained yml
            svm->load(path_savefile);
        }
        
        // Desc: Saves post-conditioned sample image to session dir and appends corresponding HoG to batch
        void sample(bool isPositive, const Mat & cropped_face)
        {
            // Compute the HOG descriptor and associate with label
            if (!batch.push(isPositive, cropped_face)) return;

            // Save the image
            string img_ext = ".png";
            if (isPositive)
            {
                cout << "p-count = " << ++positive_count << endl;
                imwrite(session_dir + "/positive/foi-" + to_string(positive_count) + img_ext, cropped_face);

#if defined(DEBUG) && defined(DEBUG_SHOW_SAMPLE)
                imshow( "Sample (+) #" + to_string(positive_count), cropped_face);
#endif
            } else
            {
                cout << "n-count = " << ++negative_count << endl;
                imwrite(session_dir + "/negative/foi-" + to_string(negative_count) + img_ext, cropped_face);

#if defined(DEBUG) && defined(DEBUG_SHOW_SAMPLE)
                imshow("Sample (-) #" + to_string(negative_count) , cropped_face);
#endif
            }                          
        }
        
        /* Desc: Grids the input frame into cells of same size as sample data. Then, creates samples of 
         *       'isPositive' type for each cell whose area is completely within frame. Basically a way 
         *       to be lazy and use one big photo instead of taking multiple photos for negative samples. 
         */
        void multiFragSample(bool isPositive, const Mat & frame, const Size & sampleSize = FACE_INPUT_WINDOW)
        {
            // Recall, Rects specify i,j from top-left corner
            for ( int row = 0; (row + sampleSize.height) < frame.rows; row += sampleSize.height)
            {
                for ( int col = 0; (col + sampleSize.width) < frame.cols; col += sampleSize.width)
                {
                    sample(isPositive, frame(Rect(Point(col, row), sampleSize)));
                }
            }
        }

        void train()
        {
#ifdef DEBUG
            batch.dump();
#endif
            
            // Make sure data can be mapped to classifier dimensions, set as current batch and train
            Mat batch_inputL1; // Updated with compat batch to make hog data input-layer compatible
            compatBatch(batch_inputL1, batch.hog_descriptors);

            // Initialize and train the SVM with the compatible input batch
            initSVM();
            svm->train(batch_inputL1, ROW_SAMPLE, Mat(batch.labels));

            // Initialize hog detector with same window size as used in samples 
            hog_detector = new HOGDescriptor;
            hog_detector->winSize = FACE_INPUT_WINDOW;

            // Get the support vectors for the SVM
            vector< float > svm_coefficients;
            Mat svectors = svm->getSupportVectors();
            int count_svectors = svectors.rows;
            
            // get the decision function
            Mat alpha, svidx;
            double rho = svm->getDecisionFunction(0, alpha, svidx);

            // Sanity checks
            CV_Assert( alpha.total() == 1 && svidx.total() == 1 && count_svectors == 1 );
            CV_Assert( (alpha.type() == CV_64F && alpha.at<double>(0) == 1.) 
                    || (alpha.type() == CV_32F && alpha.at<float>(0) == 1.f) );
            CV_Assert( svectors.type() == CV_32F );
            
            // SVM detectors expects our decision function as the last element
            svm_coefficients.resize(svectors.cols + 1);
            memcpy(&svm_coefficients[0], svectors.ptr(), svectors.cols*sizeof(svm_coefficients[0]));
            svm_coefficients[svectors.cols] = (float)-rho;
            
            // Set the coeffecients and SVM HOG interpreter 
            hog_detector->setSVMDetector( svm_coefficients );
            trained = true;
            save(PATH_SAVE_MODEL);
            
            
        }
        
        bool isTrained()
        {
            return trained;
        }
        
        void save(const string & path_savefile)
        {
            svm->save(path_savefile);
        }
 
        bool detect(const Mat & conditioned_face)
        {
            if (conditioned_face.empty()) return false;

#ifdef DEBUG
            imshow("Detective View", conditioned_face);
#endif

            // Filler data to satisfy detectMultiScale call which typically operates on an entire image
            // However, in this case the source image is just a face in question
            vector < Point > match_locations;
            hog_detector->detect( conditioned_face, match_locations);

            
            if      (0 == match_locations.size()) return false;
            else if (1 == match_locations.size()) return true;
            else
            {
                warning(__func__, "Multiple matches in an image which should just be a single face");
                return false;
            }
        }
    
};


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
    void drawFacesOn(Mat & img_display)
    {
        // Draw faces onto img_display frame
        for (int i = 0; i < vfaces_in_frame.size(); i++)
        {
            Point top_left(vfaces_in_frame[i].x, vfaces_in_frame[i].y );
            Point bottom_right(vfaces_in_frame[i].x + vfaces_in_frame[i].width, 
                               vfaces_in_frame[i].y + vfaces_in_frame[i].height);
            
            // Make the first discovered face the Face of Interest
            if(i == 0) rectangle(img_display, top_left, bottom_right, Scalar(0,255, 0), 4, 8, 0 );
            else       rectangle(img_display, top_left, bottom_right, Scalar(255,0, 0), 4, 8, 0 );
        }
    }

    // Desc : Update faces vector with faces in given frame 
    void findFaces(const Mat & finput)
    {
        cascade.detectMultiScale(mcurrent_frame_conditioned, 
                                 vfaces_in_frame, 
                                 1.1, 2, 
                                 0|CV_HAAR_SCALE_IMAGE, 
                                 Size(30, 30));
    }
    
    public:
        HaarClassifier(string path2coefficients = "")
        {
            Mat mcurrent_frame_conditioned = Mat();
            vector<Rect> vfaces_in_frame   = vector <Rect>();// Size 16
            CascadeClassifier cascade      = CascadeClassifier();
            
            if (path2coefficients.size() > 0) load(path2coefficients);
        }
        // Desc: Take some input image, find the faces and update the capture faces this
        //       HAAR classifier knows of, then draw those faces on some temporary image and show it
        void showFaces(const Mat & img_2check)
        {
            // Read current frame, dont bother processing bad frames
            if (img_2check.empty()) return;

            // Massage the input a bit, and store for reference
            neatralizeInput(img_2check, mcurrent_frame_conditioned);

            // Update faces vector with those in current frame 
            findFaces(img_2check);

            // Overlay the discovered faces on input image and show it
            Mat display = img_2check.clone();
            drawFacesOn(display);
            imshow("[HAAR Classifier]: Faces In Frame", display);
        }

        bool load(string path_cascades_yml)
        {
            cout << "Loading HAAR data from\n    '" << path_cascades_yml << "'\n";
            if( !cascade.load(path_cascades_yml) )
            { 
                error(__func__, "Couldn't load HAAR coefficients file"); 
                return false; 
            }
            else return true;
        }
        
        // Desc: Return main face of interest, the face to be sampled. Additionally, resizes face
        //       to be of same size as those used to train the SVM
        Mat getFOI()
        {
            Mat regularized_foi;
            if (1 == vfaces_in_frame.size())
            {
                   resize(mcurrent_frame_conditioned(vfaces_in_frame[0]), regularized_foi, FACE_INPUT_WINDOW); 
            }

            return regularized_foi;
        }
        
        const Mat & getFrame()
        {
            return mcurrent_frame_conditioned;
        }

        void saveFrame(const string & path)
        {
            imwrite(path, mcurrent_frame_conditioned);
        }

};



string getStateName(State_t state)
{
    switch(state)
    {
        case s_Setup  : return "Setup";
        case s_Sample : return "Sampling";
        case s_Detect : return "Detecting";
        case s_Select : return "Selecting";
        case s_Exit   : return "Exiting";
        default       : error(__FUNCTION__, "Corrupted state!");
    }
    return "Corrupted";
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



//---------------------------------------------------------------------------------------------------------------
//                                                    MAIN
//---------------------------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    // Open up camera
    VideoCapture video(0);
    if(!cameraCheck( video )) return -1;
    

    // Load up face detection cascades for face localization HAAR
    HaarClassifier haar = HaarClassifier();
    if (!haar.load( PATH_FACE_CASCADE )) return -1;

    // Prepare Face Classifier
    FaceIdentifier detective = FaceIdentifier(PATH_SAMPLE_DIR, FILENAME_SESSION_DATA);

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

        // No matter the state, let's show user's what face is being tracked
        haar.showFaces(current_frame);

        // FSM
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
                        detective.sample(true, haar.getFOI());
                        
                        break;
                    }
                    case 'n':
                    {
                        // Split up the current frame into as many subwindows of same size as rest of samples
                        // TODO: do fragmentSample. Get conditioned input frame from HAAR
                        // TODO: Verify the above, now implemented, works as intended
                        detective.multiFragSample(false, haar.getFrame());

                        break;
                    }
                    case 'f':
                    {
                        static int test_count = 0;
                        string tmp = "test_samples/t";
                        haar.saveFrame(tmp + to_string(test_count++) + ".png");
                        break;
                    }
                       
                }
                break;
            }
            case(s_Detect):
            {
                if (!detective.isTrained()) detective.train();
                if (detective.detect(haar.getFOI())) cout << "Found David" << endl;
                else                                 cout << "Searching..." << endl;
                
                break;
            }
            case(s_Select):
            {
                break;
            }
            case(s_Exit):
            {
                // Save training session
                detective.saveSession();

                return 0;
            }
            default:
            {
                error(__FUNCTION__, "Corrupted State!");
                return 1;
            }
        }
    }
}



// From http://www.juergenwiki.de/work/wiki/doku.php?id=public:hog_descriptor_computation_and_visualization
Mat get_hogdescriptor_visu(const Mat& color_origImg, vector<float>& descriptorValues, const Size & size )
{
    const int DIMX = size.width;
    const int DIMY = size.height;
    float zoomFac = 3;
    Mat visu;
    resize(color_origImg, visu, Size( (int)(color_origImg.cols*zoomFac), (int)(color_origImg.rows*zoomFac) ) );

    int cellSize        = 8;
    int gradientBinSize = 9;
    float radRangeForOneBin = (float)(CV_PI/(float)gradientBinSize); // dividing 180 into 9 bins, how large (in rad) is one bin?

    // prepare data structure: 9 orientation / gradient strenghts for each cell
    int cells_in_x_dir = DIMX / cellSize;
    int cells_in_y_dir = DIMY / cellSize;
    float*** gradientStrengths = new float**[cells_in_y_dir];
    int** cellUpdateCounter   = new int*[cells_in_y_dir];
    for (int y=0; y<cells_in_y_dir; y++)
    {
        gradientStrengths[y] = new float*[cells_in_x_dir];
        cellUpdateCounter[y] = new int[cells_in_x_dir];
        for (int x=0; x<cells_in_x_dir; x++)
        {
            gradientStrengths[y][x] = new float[gradientBinSize];
            cellUpdateCounter[y][x] = 0;

            for (int bin=0; bin<gradientBinSize; bin++)
                gradientStrengths[y][x][bin] = 0.0;
        }
    }

    // nr of blocks = nr of cells - 1
    // since there is a new block on each cell (overlapping blocks!) but the last one
    int blocks_in_x_dir = cells_in_x_dir - 1;
    int blocks_in_y_dir = cells_in_y_dir - 1;

    // compute gradient strengths per cell
    int descriptorDataIdx = 0;
    int cellx = 0;
    int celly = 0;

    for (int blockx=0; blockx<blocks_in_x_dir; blockx++)
    {
        for (int blocky=0; blocky<blocks_in_y_dir; blocky++)
        {
            // 4 cells per block ...
            for (int cellNr=0; cellNr<4; cellNr++)
            {
                // compute corresponding cell nr
                cellx = blockx;
                celly = blocky;
                if (cellNr==1) celly++;
                if (cellNr==2) cellx++;
                if (cellNr==3)
                {
                    cellx++;
                    celly++;
                }

                for (int bin=0; bin<gradientBinSize; bin++)
                {
                    float gradientStrength = descriptorValues[ descriptorDataIdx ];
                    descriptorDataIdx++;

                    gradientStrengths[celly][cellx][bin] += gradientStrength;

                } // for (all bins)


                // note: overlapping blocks lead to multiple updates of this sum!
                // we therefore keep track how often a cell was updated,
                // to compute average gradient strengths
                cellUpdateCounter[celly][cellx]++;

            } // for (all cells)


        } // for (all block x pos)
    } // for (all block y pos)


    // compute average gradient strengths
    for (celly=0; celly<cells_in_y_dir; celly++)
    {
        for (cellx=0; cellx<cells_in_x_dir; cellx++)
        {

            float NrUpdatesForThisCell = (float)cellUpdateCounter[celly][cellx];

            // compute average gradient strenghts for each gradient bin direction
            for (int bin=0; bin<gradientBinSize; bin++)
            {
                gradientStrengths[celly][cellx][bin] /= NrUpdatesForThisCell;
            }
        }
    }

    // draw cells
    for (celly=0; celly<cells_in_y_dir; celly++)
    {
        for (cellx=0; cellx<cells_in_x_dir; cellx++)
        {
            int drawX = cellx * cellSize;
            int drawY = celly * cellSize;

            int mx = drawX + cellSize/2;
            int my = drawY + cellSize/2;

            rectangle(visu, Point((int)(drawX*zoomFac), (int)(drawY*zoomFac)), Point((int)((drawX+cellSize)*zoomFac), (int)((drawY+cellSize)*zoomFac)), Scalar(100,100,100), 1);

            // draw in each cell all 9 gradient strengths
            for (int bin=0; bin<gradientBinSize; bin++)
            {
                float currentGradStrength = gradientStrengths[celly][cellx][bin];

                // no line to draw?
                if (currentGradStrength==0)
                    continue;

                float currRad = bin * radRangeForOneBin + radRangeForOneBin/2;

                float dirVecX = cos( currRad );
                float dirVecY = sin( currRad );
                float maxVecLen = (float)(cellSize/2.f);
                float scale = 2.5; // just a visualization scale, to see the lines better

                // compute line coordinates
                float x1 = mx - dirVecX * currentGradStrength * maxVecLen * scale;
                float y1 = my - dirVecY * currentGradStrength * maxVecLen * scale;
                float x2 = mx + dirVecX * currentGradStrength * maxVecLen * scale;
                float y2 = my + dirVecY * currentGradStrength * maxVecLen * scale;

                // draw gradient visualization
                line(visu, Point((int)(x1*zoomFac),(int)(y1*zoomFac)), Point((int)(x2*zoomFac),(int)(y2*zoomFac)), Scalar(0,255,0), 1);

            } // for (all bins)

        } // for (cellx)
    } // for (celly)


    // don't forget to free memory allocated by helper data structures!
    for (int y=0; y<cells_in_y_dir; y++)
    {
        for (int x=0; x<cells_in_x_dir; x++)
        {
            delete[] gradientStrengths[y][x];
        }
        delete[] gradientStrengths[y];
        delete[] cellUpdateCounter[y];
    }
    delete[] gradientStrengths;
    delete[] cellUpdateCounter;

    return visu;

} // get_hogdescriptor_visu
