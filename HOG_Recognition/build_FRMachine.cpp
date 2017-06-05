//-------------------------------------------------------------------------------------------------
//  AUTH: David Chalco
//  DESC: Facal Recognition Example
//  DATE: 06-04-2017
//  NOTE: See similar matlab ex. :""
//-------------------------------------------------------------------------------------------------
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
//-------------------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
//-------------------------------------------------------------------------------------------------
void show_instructions(void);
void generate_DB_HOGs(char* db_path, char* sample_file, int Nsamples);
//-------------------------------------------------------------------------------------------------
int main(int argc, char** argv) { 
    // NOTE: Unstable! Will crash for numbers with more than 24-digits
    char sample_name[32] = {0};
    char sample_info[64] = {0};

    // Activate default native camera
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;

    // Show instructions
    show_instructions();

    // Load up training data info
    int Nsamples = -1;
    FILE* f_trainingInfo = fopen("./training_samples/info.txt", "r");
    assert(f_trainingInfo != NULL);
    fscanf(f_trainingInfo, "Number of samples = %d\n", &Nsamples);
    fclose(f_trainingInfo);
    
    // Start video capture
    Mat bw_sample, frame;
    namedWindow("edges",1);
    for(;;)
    {
        cap >> frame;         // Capture frame
        cvtColor(frame, bw_sample, COLOR_BGR2GRAY);
        imshow("Press 'c' to capture sample", bw_sample);
    
        switch(waitKey(30)) {
          case 'q':
          {
              // Save modified number of samples
              sprintf(sample_info, "Number of samples = %d\n", Nsamples);
              FILE* f_trainingInfo = fopen("./training_samples/info.txt", "w");
              assert(f_trainingInfo != NULL);
              
              // Collect HOG data
              generate_DB_HOGs("./training_samples", "dcface_", 1);
              fprintf(f_trainingInfo, sample_info);
              fclose(f_trainingInfo);
              return 0;
          }
          case 'c':
          {   
              // Sample image captured, store to training data
              sprintf(sample_name, "./training_samples/dcface_%d.png", ++Nsamples);
              imwrite(sample_name, bw_sample); 
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
