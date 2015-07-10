//
//  Utilities.cpp
//  MoMa-ExtractionTool
//
//  Created by Tom Runia on 05/03/15.
//  Copyright (c) 2015 TUDelft. All rights reserved.
//

#include "Utilities.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>

namespace utilities
{

    std::string toLowerCase(const std::string& inputString)
    {
        std::string t;
        for (std::string::const_iterator i = inputString.begin(); i != inputString.end(); ++i)
        {
            t += tolower(*i);
        }
        return t;
    }
    
    std::string getFilename(const std::string& fullPath)
    {
        unsigned pos = (unsigned)fullPath.find_last_of("/\\");
        std::string filename = fullPath.substr(pos+1);
        return filename;
    }
    
    std::string getFilenameWithoutExtension(const std::string& fullPath)
    {
        std::string withExtension = getFilename(fullPath);
        std::string::size_type lastDot = withExtension.find_last_of(".");
        if (lastDot == std::string::npos) return fullPath;
        return withExtension.substr(0, lastDot);
    }
    
    std::string getExtensionFromFilename (const std::string& fullPath)
    {
        return fullPath.substr(fullPath.find_last_of(".") + 1);
    }
    
    std::string nextFilename (const std::string& directory)
    {
        std::vector<std::string> files;
        getFilesInDirectory(directory, files, "png");
        
        if (files.empty()) return "000001.png";
        
        std::string lastFilename = files[files.size()-1];
        int posLastSlash = (int)lastFilename.find_last_of("/");
        
        std::string numberFilename = lastFilename.substr(posLastSlash+1, 6);
        numberFilename.erase(0, numberFilename.find_first_not_of('0'));
        int nextIndex = stoi(numberFilename) + 1;
        
        char buffer[100];
        sprintf(buffer, "%06d.png", nextIndex);
        files.clear();
        return std::string(buffer);
    }
    
    void createDirectory (const std::string& path)
    {
        mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    
    void getFilesInDirectory(const std::string& directory, std::vector<std::string>& fileNames,
                             const std::vector<std::string>& validExtensions, const bool sortFilenames)
    {
        struct dirent* ep;
        size_t extensionLocation;
        DIR* dp = opendir(directory.c_str());
        
        if (dp != NULL) {
            while ((ep = readdir(dp)))
            {
                // Ignore (sub-)directories like
                if (ep->d_type & DT_DIR) {
                    continue;
                }
                
                extensionLocation = std::string(ep->d_name).find_last_of("."); // Assume the last point marks beginning of extension like file.ext
                // Check if extension is matching the wanted ones
                std::string tempExt = toLowerCase(std::string(ep->d_name).substr(extensionLocation + 1));
                
                // Check for matching file
                if (find(validExtensions.begin(), validExtensions.end(), tempExt) != validExtensions.end()) {
                    fileNames.push_back((std::string)directory + ep->d_name);
                }
            }
            (void)closedir(dp);
        }
        
        if (sortFilenames)
        {
            // Make sture the filenames are sorted
            std::sort(fileNames.begin(), fileNames.end());
        }
        
    }
    
    void getFilesInDirectory(const std::string& directory, std::vector<std::string>& fileNames,
                             const std::string& validExtension, const bool sortFilenames)
    {
        getFilesInDirectory(directory, fileNames, std::vector<std::string>{validExtension}, sortFilenames);
    }
    
}


