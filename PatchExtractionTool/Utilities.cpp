//
//  Utilities.cpp
//  PatchExtractionTool
//
//  Created by Tom Runia on 07/02/15.
//

#include "Utilities.h"

namespace utilities
{
    //*************************************************************//
    //****************** GENERAL HELPER METHODS *******************//
    //*************************************************************//
    
    string toLowerCase(const string& inputString)
    {
        string t;
        for (string::const_iterator i = inputString.begin(); i != inputString.end(); ++i) {
            t += tolower(*i);
        }
        return t;
    }
    
    string getFilename(const string& fullPath)
    {
        unsigned pos = (unsigned)fullPath.find_last_of("/\\");
        string filename = fullPath.substr(pos+1);
        return filename;
    }
    
    string getFilenameWithoutExtension(const string& fullPath)
    {
        string withExtension = getFilename(fullPath);
        int lastDot = withExtension.find_last_of(".");
        if (lastDot == string::npos) return fullPath;
        return withExtension.substr(0, lastDot);
    }
    
    string nextFilename (const string& directory)
    {
        vector<string> files;
        getFilesInDirectory(directory, files, "png");
        int index = (int)files.size();
        char buffer[100];
        sprintf(buffer, "%06d.png", index);
        files.clear();
        return string(buffer);
    }
    
    void createDirectory (const string& path)
    {
        mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    
    void getFilesInDirectory(const string& directory, vector<string>& fileNames, const vector<string>& validExtensions)
    {
        struct dirent* ep;
        size_t extensionLocation;
        DIR* dp = opendir(directory.c_str());
        
        if (dp != NULL) {
            while ((ep = readdir(dp))) {
                // Ignore (sub-)directories like
                if (ep->d_type & DT_DIR) {
                    continue;
                }
                extensionLocation = string(ep->d_name).find_last_of("."); // Assume the last point marks beginning of extension like file.ext
                // Check if extension is matching the wanted ones
                string tempExt = toLowerCase(string(ep->d_name).substr(extensionLocation + 1));
                
                // Check for matching file
                if (find(validExtensions.begin(), validExtensions.end(), tempExt) != validExtensions.end()) {
                    fileNames.push_back((string)directory + ep->d_name);
                }
            }
            (void)closedir(dp);
        }
        // Make sture the filenames are sorted
        sort(fileNames.begin(), fileNames.end());
    }
    
    void getFilesInDirectory(const string& directory, vector<string>& fileNames, const string& validExtension)
    {
        getFilesInDirectory(directory, fileNames, vector<string>{validExtension});
    }
    
    //*************************************************************//
    //*************** OPENCV RELATED HELPER METHODS ***************//
    //*************************************************************//
    
    void resizeImage (const cv::Mat& input, cv::Mat& output, cv::Size target)
    {
        // Interpolation mode (shrinking versus enlarging)
        int interpolationMode = (target.width > input.cols || target.height > input.rows) ?
        cv::INTER_CUBIC : cv::INTER_AREA;
        output = cv::Mat::zeros(target, input.type());
        cv::resize(input, output, output.size(), 0, 0, interpolationMode);
    }
}
