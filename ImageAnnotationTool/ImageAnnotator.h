//
//  ImageAnnotator.h
//  PatchExtractionTool
//
//  Created by Tom Runia on 10/07/15.
//  Copyright (c) 2015 TUDelft. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <opencv2/core/core.hpp>

class ImageAnnotator
{
    
public:
    
    ImageAnnotator();
    void labelImagesInDirectory (const std::string& dir,
                                 const std::string& annotation_file,
                                 const std::string& patch_dir = "");
    
private:
    
    void redrawAnnotations ();
    void writeAnnotationsToFile ();
    void writeAnnotationImages ();
    void adjustRectangleSize (cv::Rect& r, int dx, int dy);
    
    void keyPressHandler (int keypress);
    void mouseCallBackHandler (int event, int x, int y);
    static void mouseCallBackHandler (int event, int x, int y, int flags, void* userdata);
    
    std::string _patch_dir;
    bool _selection_active;
    bool _next_image;
    int  _image_index;
    std::vector<std::string> _image_files;
    std::vector<cv::Rect> _annotations;
    
    cv::Mat _image;
    cv::Mat _image_with_annotations;
    
    std::ofstream _output_file;
    
};
