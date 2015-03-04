//
//  main.cpp
//  PatchExtractionTool
//
//  Created by Tom Runia on 04/03/15.
//  Copyright (c) 2015 TUDelft. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <vector>
#include <map>
#include <list>
#include <string>
#include <unistd.h>
#include <ctime>
#include <assert.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "Utilities.h"

using namespace std;
using namespace utilities;

void resetSelection();
void selectionToRectangle(bool makeSquareSize = false);
void writePatchAsImage();
void writePatchToCascadeFile();

cv::Mat image, imageWithSelection;

int key;
int imageIndex = 0;
bool startSelection = false;
bool readyForExtraction = false;
bool nextImageUponSave = true;

cv::Point p1 (0, 0);
cv::Point p2 (0, 0);
cv::Rect roi;

cv::Size targetSize;

vector<string> imageFiles;
string inputPath, outputPath;

ofstream filestream;

void keyPressHandler () {
    //cout << key << endl;
    
    switch(key) {
        case 32: // 'space' (save image)
            writePatchAsImage();
            writePatchToCascadeFile();
            resetSelection();
            if (nextImageUponSave) imageIndex++;
            break;
            
        case 117: // 'u' (undo selection)
            resetSelection();
            break;
        
        case 97: // 'a' (previous image)
            imageIndex--;
            break;
            
        case 100: // 'd' (next image)
            imageIndex++;
            break;
            
        case 27:  // 'esc' (exit application)
        case 113: // 'q'   (exit application)
            if (filestream.is_open()) filestream.close();
            exit(EXIT_SUCCESS);
    }
}

void mouseCallBackHandler (int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN && !startSelection) {
        
        // First mouse click, initialize drawing ROI
        p1.x = x;
        p1.y = y;
        startSelection = true;

    } else if (event == cv::EVENT_LBUTTONUP && startSelection) {
        
        // Second mouse click, patch is ready for extraction
        p2.x = x;
        p2.y = y;
        
        // Scale to square size
        selectionToRectangle(false);
        startSelection = false;
        
        imageWithSelection = image.clone();
        cv::rectangle(imageWithSelection, p1, p2, cv::Scalar(0, 255, 0), 1);
        cv::imshow("Patch Extraction Tool", imageWithSelection);
        
    } else if (event == cv::EVENT_MOUSEMOVE && startSelection) {
        p2.x = x;
        p2.y = y;
        
        imageWithSelection = image.clone();
        cv::rectangle(imageWithSelection, p1, p2, cv::Scalar(255, 255, 255), 1);
        cv::imshow("Patch Extraction Tool", imageWithSelection);
    }
}

void resetSelection () {
    // Reset status
    startSelection = false;
    readyForExtraction = false;
    p1.x = 0; p1.y = 0;
    p2.x = 0; p2.y = 0;
}

void selectionToRectangle (bool makeSquareSize)
{
    if (!makeSquareSize) {
        roi.x = p1.x;
        roi.y = p1.y;
        roi.width = abs(p2.x-p1.x);
        roi.height = abs(p2.y-p1.y);
        
        readyForExtraction = true;
        return;
    }
    
    // Rescale selection to make it square
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    
    double margin = abs(dx-dy) / 2.0;
    
    if (dx >= dy) {
        // Landscape
        p1.y -= margin;
        p2.y += margin;
    } else {
        // Portrait
        p1.x -= margin;
        p2.x += margin;
    }
    
    roi.x = p1.x;
    roi.y = p1.y;
    roi.width = abs(p2.x-p1.x);
    roi.height = abs(p2.y-p1.y);
    
    if (roi.x < 0 || roi.y < 0 || (roi.x+roi.width) >= image.cols || (roi.y+roi.height) >= image.rows) {
        cout << "Patch falls outside image, select again." << endl;
        resetSelection();
    } else {
        readyForExtraction = true;
    }
}

void writePatchAsImage () {
    if (!readyForExtraction) return;
    
    cv::Mat patch = image(roi);
    cv::resize(patch, patch, targetSize);
    
    string filename = nextFilename(outputPath);
    printf("Writing '%s' to disk...\n", filename.c_str());
    cv::imwrite(outputPath + filename, patch);
}

// Write ROI to cascade file 'info.dat'
void writePatchToCascadeFile ()
{
    if (!readyForExtraction) return;
    
    if (!filestream.is_open()) {
        cout << "Unable to write to output file." << endl;
        return;
    }

    // Write line to file containing image path and bounding boxes (x y width height)
    filestream << imageFiles[imageIndex] << " 1 ";
    filestream << (int)floor(roi.x) << " ";
    filestream << (int)floor(roi.y) << " ";
    filestream << (int)ceil(roi.width) << " ";
    filestream << (int)ceil(roi.height) << "\n";
    
    cout << "Saved ROI to cascade file." << endl;
}

int main(int argc, const char * argv[])
{
    
    inputPath = "/Users/tomrunia/Development/TomTom/data/TrainingData/TrafficSigns/train/";
    outputPath = "/Users/tomrunia/Development/TomTom/data/TrainingData/TrafficSigns/triangle-signs/";
    
    // Retrieve image files from directory
    getFilesInDirectory(inputPath, imageFiles, vector<string>{"png", "jpg", "gif", "bmp"});
    
    if (imageFiles.empty()) {
        cout << "Could not find images in input directory...EXIT" <<endl;
        return EXIT_FAILURE;
    }
    
    // If not existing, create directory for saving images
    createDirectory(outputPath);
    
    string cascadeFile = "/Users/tomrunia/Development/TomTom/data/TrainingData/TrafficSigns/traffic-signs-triangle.dat";
    filestream.open(cascadeFile, ios::out);
    
    targetSize.width = 100;
    targetSize.height = 100;
    
    // Create OpenCV window and attach mouse listener
    cv::namedWindow("Patch Extraction Tool", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Patch Extraction Tool", mouseCallBackHandler, NULL);
    
    // Iterate through all images in the directory
    while (imageIndex <= imageFiles.size()) {
        image = cv::imread(imageFiles[imageIndex], cv::IMREAD_UNCHANGED);
        imageWithSelection = image.clone();
        
        cv::imshow("Patch Extraction Tool", image);
        
        key = cv::waitKey(0);
        keyPressHandler();
    }
    
    cout << "No more images in directory...DONE" << endl;
    return EXIT_SUCCESS;
}
