//-------------------------------------------------------------------------------------------------
//  AUTH: David Chalco
//  DESC: Facal Recognition Example
//  DATE: 06-04-2017
//  NOTE: See similar matlab ex. :""
//-------------------------------------------------------------------------------------------------
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <time.h>
//-------------------------------------------------------------------------------------------------
#define PATH_FACE_CASCADE "./HAAR_faceData/haarcascade_frontalface_alt.xml"
#define DEBUG
//-------------------------------------------------------------------------------------------------
using namespace cv;
using namespace cv::ml;
using namespace std;
//-------------------------------------------------------------------------------------------------
void show_instructions(void);
void generate_DB_HOGs(char* db_path, char* sample_file, int Nsamples);
Mat get_hogdescriptor_visu(const Mat & color_origImg, vector < float > & descriptorValues, const Size & size );
vector <Rect> detectAndDisplay( Mat frame, Mat frame_gray, CascadeClassifier face_cascade );
void train_svm( const vector< Mat > & gradient_lst, const vector< int > & labels );
void push_HOG(vector < Mat > & gradients, Mat & roi, HOGDescriptor & hog, vector < Point > & location, vector < float > & descriptors); 
void convert_to_ml(const std::vector< cv::Mat > & train_samples, cv::Mat& trainData );
//-------------------------------------------------------------------------------------------------
int main(int argc, char** argv) { 
    // NOTE: Unstable! Will crash for numbers with more than 24-digits
    char sample_name[32] = {0};

    // Activate default native camera
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;

    // Show instructions
    show_instructions();

    // Load up training data info HOG
    int NPsamples = -1;
    int NNsamples = -1;
    FILE* f_trainingInfo = fopen("./training_samples/info.txt", "r");
    assert(f_trainingInfo != NULL);
    fscanf(f_trainingInfo, "Number of p-samples = %d\nNumber of n-samples = %d\n", &NPsamples, &NNsamples);
    fclose(f_trainingInfo);

    // Load up face detection cascades for face localization HAAR
    CascadeClassifier face_cascade;
    if( !face_cascade.load( PATH_FACE_CASCADE ) )
    { 
        printf("--(!)Error loading\n"); 
        return -1; 
    }

    // Start video capture
    vector < Mat > gradients;
    vector < int > labels;
    Mat bw_sample, frame;
    vector< Rect > face;
    
    // HOG variables
    Mat gray;
    HOGDescriptor hog;
    vector< Point > location;
    vector< float > descriptors; 
    for(;;)
    {
        cap >> frame;                   // Capture frame
        if (frame.empty()) continue;    // Skipe empty frames
        
        // Get gray scale image and Histogram Equalize to diminish lighting influences
        cvtColor( frame, bw_sample, CV_BGR2GRAY );
        equalizeHist( bw_sample, bw_sample);
        
        // Detect faces
        face = detectAndDisplay( frame, bw_sample, face_cascade);
        imshow("Press 'c' to capture sample", frame);
        switch(waitKey(30)) {
            case 'q':
            {
                // Save modified number of samples
                FILE* f_trainingInfo = fopen("./training_samples/info.txt", "w+");
                assert(f_trainingInfo != NULL);
                
                // Collect HOG data
                //generate_DB_HOGs("./training_samples", "dcface_", 1);
                // CONTINUE HERE : Finish SVM training
                train_svm(gradients, labels);

                fprintf(f_trainingInfo, "Number of p-samples = %d\nNumber of n-samples = %d\n", NPsamples, NNsamples);
                fclose(f_trainingInfo);
                return 0;
            }
            case 'p':
            {   
                // Crop/Sample the detected face, store the HEQ-GrayScale cropped image for training data
                sprintf(sample_name, "./training_samples/dcface_%d.png", ++NPsamples);
                Mat cropped_face = bw_sample(face[0]);  // Gray scale and Histogram-Equalized

                // Show the cropped out, histogram-equalized face
                imshow("Grayscale & Histogram-Equalized Sampled face", cropped_face);
                imwrite(sample_name, cropped_face);
                printf("Face located @ (%d,%d)\n", face[0].x, face[0].y);
                printf("Size of image = %dx%d\n", face[0].width, face[0].height);
                
                // Append another HOG Entry and assign label
                push_HOG(gradients, cropped_face, hog, location, descriptors);
                labels.push_back(+1);

                // Visualize HOG descriptors
                imshow( "HOG visual of sample", get_hogdescriptor_visu( cropped_face.clone(), descriptors, hog.winSize ) );
                break;
            }
            case 'n':
            {

                
                // Process any/all captured faces
                Mat neg_face;
                for (int i=0; i<face.size(); i++)
                {
                    neg_face = bw_sample(face[i]);
                    
                    // Crop/Sample the detected face, store the HEQ-GrayScale cropped image for training data
                    sprintf(sample_name, "./training_samples/neg_%d.png", ++NNsamples);
                    imshow("Negative captured face", neg_face);
                    imwrite(sample_name, neg_face);

                    // Append to negative HOG descriptors
                    push_HOG(gradients, neg_face, hog, location, descriptors);
                    labels.push_back(-1);

                    #ifdef DEBUG
                    imshow( "HOG visual of negative captured face", get_hogdescriptor_visu( neg_face.clone(), descriptors, hog.winSize ) );
                    #endif
                }

                // Subdivide the whole frame into subsets of the HOG window size for more negative images
                // NOTE: This was intended for use with cameras that capture images bigger than hog window size = 128 x 128
                Mat sub_img;
                for (int i=0; i<bw_sample.size().width - 128; i+=127)
                {
                    for (int j=0; j<bw_sample.size().height - 128; j+=127)
                    {
                        // Extract subimage, HOG process, and store
                        sub_img = bw_sample(Rect(i,j,128,128));
                        printf("i = %d\nj = %d\n", i,j);

                        // Crop/Sample the detected face, store the HEQ-GrayScale cropped image for training data
                        sprintf(sample_name, "./training_samples/neg_%d.png", ++NNsamples);
                        imshow("(-) Sub-image", sub_img);
                        imwrite(sample_name, sub_img);
                        
                        // Generate negative HOG data for each sub image in background
                        push_HOG(gradients, sub_img, hog, location, descriptors);
                        labels.push_back(-1);

                        #ifdef DEBUG
                        imshow( "HOG visual of (-) sub image", get_hogdescriptor_visu( sub_img.clone(), descriptors, hog.winSize ) );
                        #endif
                    }
                }
                break;
            }
        }
    }
    // Close files
    fclose(f_trainingInfo);

    return 0;
}
//-------------------------------------------------------------------------------------------------
void show_instructions() {
    FILE* f_insn = fopen("./instruct.txt", "r+");
    char buf[64] = {0};
    
    assert(f_insn != NULL);
    while(fgets(buf, sizeof(buf), f_insn)) printf("%s",buf);
    fclose(f_insn); 
}

// See: http://study.marearts.com/2014/04/example-source-code-of-extract-hog.html
// Generates hog files
void generate_DB_HOGs(char* db_path, char* sample_file, int Nsamples) {
    vector <vector <float> > v_descriptorsValues;
    vector <vector <Point> > v_locations;
    
    char* sample_path = (char*)malloc(strlen(db_path) + strlen(sample_file)+3+1); // Limited to 3 digits
    // Iterate through all training samples
    for(int i=1; i<=Nsamples; i++)
    {   
        // Generate HOG data from image
        sprintf(sample_path, "%s/%s%d.png", db_path, sample_file, i); 
        Mat sample_img = imread(sample_path, CV_LOAD_IMAGE_GRAYSCALE);
        resize(sample_img, sample_img, Size(64,48) ); // Note: Other sizes possible
        HOGDescriptor d( Size(32,16), Size(8,8), Size(4,4), Size(4,4), 9);
        
        vector <float> descriptorsValues;
        vector <Point> locations;

        d.compute( sample_img, descriptorsValues, Size(0,0), Size(0,0), locations );
        v_descriptorsValues.push_back( descriptorsValues );
        v_locations.push_back( locations );
    }
    
    // Save HOG data for training data
    FileStorage hogXml("./training_HOGs/positive.xml", FileStorage::WRITE);
    int row = v_descriptorsValues.size(), col=v_descriptorsValues[0].size();
    printf("col=%d, row=%d\n", row, col);
    Mat M(row,col,CV_32F);

    //save Mat to XML
    for(int i=0; i<row; ++i)
    {
        memcpy( &(M.data[col * i * sizeof(float) ]) ,v_descriptorsValues[i].data(),col*sizeof(float));
    }

    //write xml
    write(hogXml, "Descriptor_of_images", M);
    hogXml.release();
    
    // Free up memory
    free(sample_path);
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

// Uses HAAR cascades to detect faces. Also returns the Grayscale convervted frame
vector <Rect> detectAndDisplay(Mat frame, Mat frame_gray, CascadeClassifier face_cascade )
{
  std::vector<Rect> faces;

  //-- Detect faces
  face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
  if ( faces.size() > 1) 
  {
      printf("Only one face at time!\n");
      return vector<Rect>();
  } 
  
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

    svm->save( "myface_SVMdetector.yml" );
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

void push_HOG(vector < Mat > & gradients, Mat & roi, HOGDescriptor & hog, vector < Point > & location, vector < float > & descriptors) 
{
    // Resize for uniform comparison
    resize(roi, roi, Size(128,128) );
    hog.winSize = roi.size();

    // Compute HOG data and append to gradient list
    hog.compute( roi, descriptors, Size(32,32), Size(0,0), location );
    gradients.push_back( Mat(descriptors).clone() );
}
