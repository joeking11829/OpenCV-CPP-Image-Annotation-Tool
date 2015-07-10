//
//  Utilities.h
//  MoMa-ExtractionTool
//
//  Created by Tom Runia on 05/03/15.
//  Copyright (c) 2015 TUDelft. All rights reserved.
//

#pragma once

#include <string>
#include <vector>

namespace utilities
{
    
    std::string toLowerCase(const std::string& inputString);
    std::string getFilename (const std::string& fullPath);
    std::string getFilenameWithoutExtension(const std::string& fullPath);
    std::string getExtensionFromFilename (const std::string& fullPath);
    std::string nextFilename (const std::string& directory);
    void createDirectory(const std::string& path);
    
    void getFilesInDirectory(const std::string& directory,
                             std::vector<std::string>& fileNames,
                             const std::vector<std::string>& validExtensions,
                             const bool sortFilenames = false);
    
    void getFilesInDirectory(const std::string& directory,
                             std::vector<std::string>& fileNames,
                             const std::string& validExtension,
                             const bool sortFilenames = true);

    
}

