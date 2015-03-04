//
//  main.cpp
//  PatchExtractionTool
//
//  Created by Tom Runia on 04/03/15.
//  Copyright (c) 2015 TomTom. All rights reserved.
//

#include <iostream>
#include <vector>
#include <map>
#include <assert.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/program_options.hpp>

#include "Utilities.h"

using namespace std;
using namespace utilities;
namespace po = boost::program_options;

bool parseCommandLineParams(int argc, const char * argv[]);
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
string inputPath, outputPath, dataFile;

ofstream filestream;

void keyPressHandler () {
    switch(key) {
        case 32: // 'space' (save image)
            if (!outputPath.empty()) writePatchAsImage();
            if (!dataFile.empty()) writePatchToCascadeFile();
            resetSelection();
            if (nextImageUponSave) imageIndex++;
            break;
        
        case 27:  // 'esc' (undo selection)
        case 117: // 'u'   (undo selection)
            resetSelection();
            break;
        
        case 97: // 'a' (previous image)
            imageIndex--;
            break;
            
        case 100: // 'd' (next image)
            imageIndex++;
            break;
            
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
    
    // Optionally resize images to specified size
    if (targetSize.width != 0 && targetSize.height != 0)
        cv::resize(patch, patch, targetSize);
    
    string filename = nextFilename(outputPath);
    printf("Saved image patch '%s'.\n", filename.c_str());
    cv::imwrite(outputPath + filename, patch);
}

void writePatchToCascadeFile ()
{
    if (!readyForExtraction) return;
    
    if (!filestream.is_open()) {
        cout << "Unable to write to dataFile " << dataFile << endl;
        return;
    }

    // Write line to file containing image path and bounding boxes (x y width height)
    filestream << imageFiles[imageIndex] << " 1 ";
    filestream << (int)floor(roi.x) << " ";
    filestream << (int)floor(roi.y) << " ";
    filestream << (int)ceil(roi.width) << " ";
    filestream << (int)ceil(roi.height) << "\n";
    
    cout << "Saved ROI to data file." << endl;
}

int main(int argc, const char * argv[])
{
    bool status = parseCommandLineParams(argc, argv);
    if (!status) return EXIT_FAILURE;
    
    // Retrieve image files from directory
    getFilesInDirectory(inputPath, imageFiles, vector<string>{"png", "jpg", "gif", "bmp"});
    
    if (imageFiles.empty()) {
        cout << "Could not find images in input directory...EXIT" <<endl;
        return EXIT_FAILURE;
    }
    
    // If not existing, create directory for saving images
    if (!outputPath.empty())
        createDirectory(outputPath);
    
    if (!dataFile.empty())
        filestream.open(dataFile, ios::out);
    
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

bool parseCommandLineParams (int argc, const char * argv[])
{
    try {
        po::options_description description("Allowed options");
        
        description.add_options()
            ("input,i", po::value<string>(), "Input directory containing images (png, jpg or bmp)")
            ("patchDir,p", po::value<string>(), "Output directory for writing image patches as image.")
            ("dataFile,d", po::value<string>(), "Output file for writing bounding boxes to text file. This can be used for generating a data file suitable for the opencv_createsamples file to generate vector file.")
            ("width,w", po::value<int>(), "Width of output images (default = 100px)")
            ("height,h", po::value<int>(), "Height of output images (default = 100px)")
            ("skipSave,n", po::value<bool>(), "Directly jump to the next image when saving image patch.");
        
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
        po::notify(vm);
        
        if (!vm.count("input")) {
            cout << "Application needs input option." << endl;
            cout << "Please specify --input..." << endl;
            return false;
        } else {
            inputPath = vm["input"].as<string>();
        }
        
        if (!vm.count("patchDir") && !vm.count("dataFile")) {
            cout << "Application needs output option." << endl;
            cout << "Either specify --patchDir or --dataFile..." << endl;
            return false;
        }
        
        // Parse 'patchDir'
        if (vm.count("patchDir")) {
            outputPath = vm["patchDir"].as<string>();
        }
        
        // Parse 'dataFile'
        if (vm.count("dataFile")) {
            dataFile = vm["dataFile"].as<string>();
        }
        
        // Parse 'nextUponSave'
        if (vm.count("nextUponSave")) {
            nextImageUponSave = vm["nextUponSave"].as<bool>();
        }
        
        // Parse 'width' and 'height' of output images
        if (vm.count("width") && vm.count("height")) {
            targetSize.width =  vm["width"].as<int>();
            targetSize.height = vm["height"].as<int>();
        }
        
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return false;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
        return false;
    }
    
    return true;
}
