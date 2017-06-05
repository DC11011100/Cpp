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
Mat get_hogdescriptor_visu(const Mat& color_origImg, vector<float>& descriptorValues, const Size & size );

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
      cap >> frame;                   // Capture frame
      if (frame.empty()) continue;    // Skipe empty frames
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
              //generate_DB_HOGs("./training_samples", "dcface_", 1);
              fprintf(f_trainingInfo, sample_info);
              fclose(f_trainingInfo);
              return 0;
          }
          case 'c':
          {   
              // Sample image captured, store to training data
              sprintf(sample_name, "./training_samples/dcface_%d.png", ++Nsamples);
              imwrite(sample_name, bw_sample);
              printf("Size of image = %lux%lu\n", bw_sample.size().width, bw_sample.size().height);

              // HOG opencv ex

              const Size size = Size(1280,720);
              resize(bw_sample, bw_sample, size ); // Note: Other sizes possible
              HOGDescriptor hog;
              hog.winSize = size;
              Mat gray;
              vector< Point > location;
              vector< float > descriptors; 

              //cvtColor( bw_sample, gray, COLOR_BGR2GRAY );
              //resize(bw_sample, bw_sample, Size(1280,720) ); // Note: Other sizes possible
              hog.compute( bw_sample, descriptors, size, Size( 0, 0 ), location );
              imshow( "gradient", get_hogdescriptor_visu( bw_sample.clone(), descriptors, size ) );
              
              
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

