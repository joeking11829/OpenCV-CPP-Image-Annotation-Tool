//
//  Utilities.h
//  VisionProject
//
//  Created by Tom Runia on 07/02/15.
//

#ifndef __PatchExtractionTool__Utilities__
#define __PatchExtractionTool__Utilities__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

#include <vector>
#include <map>
#include <list>
#include <string>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

namespace utilities
{
    //*************************************************************//
    //****************** GENERAL HELPER METHODS *******************//
    //*************************************************************//
    string toLowerCase(const string& inputString);
    string getFilename (const string& fullPath);
    string getFilenameWithoutExtension(const string& fullPath);
    string nextFilename (const string& directory);
    void createDirectory(const string& path);
    void getFilesInDirectory(const string& directory, vector<string>& fileNames, const vector<string>& validExtensions);
    void getFilesInDirectory(const string& directory, vector<string>& fileNames, const string& validExtension);

    //*************************************************************//
    //*************** OPENCV RELATED HELPER METHODS ***************//
    //*************************************************************//
    void resizeImageWithAspectRatio (const cv::Mat& input, cv::Mat& output, cv::Size target);
   
}

#endif /* defined(__PatchExtractionTool__Utilities__) */
