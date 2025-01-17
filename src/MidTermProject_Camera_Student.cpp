/* INCLUDES FOR THIS PROJECT */
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <limits>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "dataStructures.h"
#include "matching2D.hpp"

#include <numeric>
using namespace std;

/* MAIN PROGRAM */
int main(int argc, const char *argv[])
{

    /* INIT VARIABLES AND DATA STRUCTURES */

    // data location
    string dataPath = "../";

    // camera
    string imgBasePath = dataPath + "images/";
    string imgPrefix = "KITTI/2011_09_26/image_00/data/000000"; // left camera, color
    string imgFileType = ".png";
    int imgStartIndex = 0; // first file index to load (assumes Lidar and camera names have identical naming convention)
    int imgEndIndex = 9;   // last file index to load
    int imgFillWidth = 4;  // no. of digits which make up the file index (e.g. img-0001.png)

    // misc
    int dataBufferSize = 2;       // no. of images which are held in memory (ring buffer) at the same time
    vector<DataFrame> dataBuffer; // list of data frames which are held in memory at the same time
    bool bVis = false;            // visualize results

    /* MAIN LOOP OVER ALL IMAGES */
    vector<int> NumberOfKeypoints;
    vector<int> NumberOfMatches;
    vector<double> runtimes;
    vector<string> DetectorTypeVector = {"SHITOMASI", "HARRIS", "FAST", "BRISK", "ORB", "AKAZE", "SIFT"};
    vector<string> DescriptorTypeVector = {"BRISK", "BRIEF", "ORB", "FREAK", "AKAZE", "SIFT"};
    string detectorType;
    string descriptorType;
    int SumOfMatches = 0;
    for (int m = 0; m < DetectorTypeVector.size(); m++)
    {
        detectorType = DetectorTypeVector[m];
        SumOfMatches = 0;
        
        descriptorType = DescriptorTypeVector[0]; // to reduce for loop, choice descriptortype one by one
        double t_total = (double)cv::getTickCount();

        for (size_t imgIndex = 0; imgIndex <= imgEndIndex - imgStartIndex; imgIndex++)
        {
            /* LOAD IMAGE INTO BUFFER */

            // assemble filenames for current index
            ostringstream imgNumber;
            imgNumber << setfill('0') << setw(imgFillWidth) << imgStartIndex + imgIndex;
            string imgFullFilename = imgBasePath + imgPrefix + imgNumber.str() + imgFileType;

            // load image from file and convert to grayscale
            cv::Mat img, imgGray;
            img = cv::imread(imgFullFilename);
            cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

            //// STUDENT ASSIGNMENT
            //// TASK MP.1 -> replace the following code with ring buffer of size dataBufferSize
            
            // push image into data frame buffer
            DataFrame frame;
            frame.cameraImg = imgGray;
            dataBuffer.push_back(frame);
            //if current buffer size > buffursize(=2), make new data buffer
            //1, 2, 3 (before) --> 2, 3 (after)
            //2, 3, 4 (before) --> 3, 4 (after)
            if(dataBuffer.size() > dataBufferSize)
            {
                dataBuffer = vector<DataFrame>(dataBuffer.end()-dataBufferSize, dataBuffer.end());
                
            }
            cout << imgIndex << "/" << imgEndIndex << endl;
            //cout << dataBuffer.size() << endl;

            //// EOF STUDENT ASSIGNMENT
            cout << "#1 : LOAD IMAGE INTO BUFFER done" << endl;

            /* DETECT IMAGE KEYPOINTS */

            // extract 2D keypoints from current image
            vector<cv::KeyPoint> keypoints; // create empty feature list for current image
            //string detectorType = "SHITOMASI";
            //string detectorType = "HARRIS";
            //string detectorType = "FAST";
            //string detectorType = "BRISK";
            //string detectorType = "ORB";
            //string detectorType = "AKAZE";
            //string detectorType = "SIFT";
            //I declared detectorType by array (line 48, line 52)


            //// STUDENT ASSIGNMENT
            //// TASK MP.2 -> add the following keypoint detectors in file matching2D.cpp and enable string-based selection based on detectorType
            //// -> SHITOMASI HARRIS, FAST, BRISK, ORB, AKAZE, SIFT
            
            bool bVis = false;

            if (detectorType.compare("SHITOMASI") == 0)
            {
                detKeypointsShiTomasi(keypoints, imgGray, bVis);
            }
            else if(detectorType.compare("HARRIS") == 0)
            {
                detKeypointsHarris(keypoints, imgGray, bVis);    
            }
            else // when other detector type selected
            {
                detKeypointsModern(keypoints, imgGray, detectorType, bVis);
            }
            
                
            
            //// EOF STUDENT ASSIGNMENT

            //// STUDENT ASSIGNMENT
            //// TASK MP.3 -> only keep keypoints on the preceding vehicle

            // only keep keypoints on the preceding vehicle
            bool bFocusOnVehicle = true;
            cv::Rect vehicleRect(535, 180, 180, 150); //cx = 535, cy = 180, w = 180, h = 150
            if (bFocusOnVehicle)
            {
                vector<cv::KeyPoint> keypoints_cropped;
                for (auto it = keypoints.begin(); it != keypoints.end(); ++it)
                {
                    int x = it->pt.x;
                    int y = it->pt.y;
                    if( (x > 535 && x < 535+180) 
                    && (y > 180 && y < 180+150) ) // if points in vehicle rectangle keep. other discard points
                    {
                        keypoints_cropped.push_back(*it);
                    }
                    
                }
                keypoints = keypoints_cropped;
            }
            cout << "detector Type: "<< detectorType << ", # of key points: " << keypoints.size() << "  (after cropped)"<<endl;
            NumberOfKeypoints.push_back(keypoints.size());

            //// EOF STUDENT ASSIGNMENT

            // optional : limit number of keypoints (helpful for debugging and learning)
            bool bLimitKpts = false;
            if (bLimitKpts)
            {
                int maxKeypoints = 50;

                if (detectorType.compare("SHITOMASI") == 0)
                { // there is no response info, so keep the first 50 as they are sorted in descending quality order
                    keypoints.erase(keypoints.begin() + maxKeypoints, keypoints.end());
                }
                cv::KeyPointsFilter::retainBest(keypoints, maxKeypoints);
                cout << " NOTE: Keypoints have been limited!" << endl;
            }

            // push keypoints and descriptor for current frame to end of data buffer
            (dataBuffer.end() - 1)->keypoints = keypoints;
            cout << "#2 : DETECT KEYPOINTS done" << endl;

            /* EXTRACT KEYPOINT DESCRIPTORS */

            //// STUDENT ASSIGNMENT
            //// TASK MP.4 -> add the following descriptors in file matching2D.cpp and enable string-based selection based on descriptorType
            //// -> BRIEF, ORB, FREAK, AKAZE, SIFT

            cv::Mat descriptors;
            //string descriptorType = "BRISK"; // BRIEF, ORB, FREAK, AKAZE, SIFT
            //string descriptorType = "BRIEF";
            //string descriptorType = "ORB";
            //string descriptorType = "FREAK";
            //string descriptorType = "AKAZE";
            //string descriptorType = "SIFT";
            //I declared descriptor Type by array (line 49)
            

            descKeypoints((dataBuffer.end() - 1)->keypoints, (dataBuffer.end() - 1)->cameraImg, descriptors, descriptorType);
            //// EOF STUDENT ASSIGNMENT

            // push descriptors for current frame to end of data buffer
            (dataBuffer.end() - 1)->descriptors = descriptors;

            cout << "#3 : EXTRACT DESCRIPTORS done" << endl;

            if (dataBuffer.size() > 1) // wait until at least two images have been processed
            {

                /* MATCH KEYPOINT DESCRIPTORS */

                vector<cv::DMatch> matches;
                string matcherType = "MAT_BF";        // MAT_BF, MAT_FLANN
                //string descriptorType = "DES_BINARY"; // DES_BINARY, DES_HOG
                //string selectorType = "SEL_NN";       // SEL_NN, SEL_KNN

                //string matcherType = "MAT_FLANN";        // MAT_BF, MAT_FLANN
                string descriptorType = "DES_HOG"; // DES_BINARY, DES_HOG
                string selectorType = "SEL_KNN";       // SEL_NN, SEL_KNN

                //// STUDENT ASSIGNMENT
                //// TASK MP.5 -> add FLANN matching in file matching2D.cpp

                //// TASK MP.6 -> add KNN match selection and perform descriptor distance ratio filtering with t=0.8 in file matching2D.cpp
                
                matchDescriptors((dataBuffer.end() - 2)->keypoints, (dataBuffer.end() - 1)->keypoints,
                                (dataBuffer.end() - 2)->descriptors, (dataBuffer.end() - 1)->descriptors,
                                matches, descriptorType, matcherType, selectorType);

                //// EOF STUDENT ASSIGNMENT

                // store matches in current data frame
                (dataBuffer.end() - 1)->kptMatches = matches;


                cout << "#4 : MATCH KEYPOINT DESCRIPTORS done" << endl;

                // visualize matches between current and previous image
                bVis = false;
                if (bVis)
                {
                    cv::Mat matchImg = ((dataBuffer.end() - 1)->cameraImg).clone();
                    cv::drawMatches((dataBuffer.end() - 2)->cameraImg, (dataBuffer.end() - 2)->keypoints,
                                    (dataBuffer.end() - 1)->cameraImg, (dataBuffer.end() - 1)->keypoints,
                                    matches, matchImg,
                                    cv::Scalar::all(-1), cv::Scalar::all(-1),
                                    vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

                    string windowName = "Matching keypoints between two camera images";
                    cv::namedWindow(windowName, 7);
                    cv::imshow(windowName, matchImg);
                    cout << "Press key to continue to next image" << endl;
                    cv::waitKey(0); // wait for key to be pressed
                }
                bVis = false;
                SumOfMatches += matches.size();
            }
            

        } // eof loop over all images               
        /*
        cout << "detectorType: "<< detectorType << endl;
        for (int z = 0; z < NumberOfKeypoints.size() ; z++)
        {
            cout << NumberOfKeypoints[z] << endl;
        }
        */

       t_total = ((double)cv::getTickCount() - t_total) / cv::getTickFrequency();
    
    cout << "detector: "<< detectorType << ", descriptor: "<< descriptorType << "  # of matches: "<< SumOfMatches << "  total runtime: " << t_total <<" s" << endl;
    }  // detector series end
    

    return 0;
    
}