//
//  ImageAnnotator.cpp
//  PatchExtractionTool
//
//  Created by Tom Runia on 10/07/15.
//  Copyright (c) 2015 TUDelft. All rights reserved.
//

#include "ImageAnnotator.h"
#include "Utilities.h"

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

ImageAnnotator::ImageAnnotator ()
{
    _selection_active = false;
    _next_image = true;
    _image_index = 0;
}

void ImageAnnotator::labelImagesInDirectory (const std::string& dir,
                                             const std::string& annotation_file,
                                             const std::string& patch_dir)
{
    
    const std::vector<std::string> valid_extensions { "png", "jpg", "jpeg", "bmp" };
    utilities::getFilesInDirectory(dir, _image_files, valid_extensions, true);
    
    if (_image_files.empty())
    {
        throw std::runtime_error("Image directory is empty...");
    }
    
    // Open annotation file for writing
    _output_file.open(annotation_file, std::ios::out);
    
    if (!_output_file.is_open())
    {
        throw std::runtime_error("Unable to open annotation file for writing...");
    }
    
    // Optionally set output dir for saving annotations as image (PNG)
    _patch_dir = patch_dir;
    
    cv::namedWindow("Image Annotator", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Image Annotator", mouseCallBackHandler, this);
    
    std::string image_file;
    
    while (_image_index < (int)_image_files.size())
    {
        
        if (_next_image)
        {
            std::printf("Labeling image %i of %i...\n", _image_index, (int)_image_files.size());
            
            image_file = _image_files.at(_image_index);
            _image = cv::imread(image_file, cv::IMREAD_COLOR);
            _image_with_annotations = _image.clone();
            _next_image = false;
        }
        
        cv::imshow("Image Annotator", _image_with_annotations);
        
        int keypress = cv::waitKey();
        keyPressHandler(keypress);
        
    }
    
}

void ImageAnnotator::redrawAnnotations ()
{
    _image_with_annotations = _image.clone();
    
    for (const auto& r : _annotations)
    {
        cv::rectangle(_image_with_annotations, r, cv::Scalar(0, 255, 255), 2);
    }
    
    cv::imshow("Image Annotator", _image_with_annotations);
}

void ImageAnnotator::writeAnnotationsToFile ()
{
    if (_annotations.empty()) return;
    std::string filename = utilities::getFilename( _image_files.at(_image_index));
    
    for (const auto& r : _annotations)
    {
        _output_file << filename << ";" << r.x << ";" << r.y << ";";
        _output_file << r.width << ";" << r.height << std::endl;
    }
    
    std::printf("Saved %i annotations to file.\n", (int)_annotations.size());
}

void ImageAnnotator::writeAnnotationImages ()
{
    if (_annotations.empty() || _patch_dir.empty()) return;
    
    for (const auto& r : _annotations)
    {
        cv::Mat roi = _image(r);
        std::string filename = _patch_dir + utilities::nextFilename(_patch_dir);
        cv::imwrite(filename, roi);
        
    }
}

void ImageAnnotator::adjustRectangleSize (cv::Rect& r, int dx, int dy)
{
    r.x -= (int)std::round(dx / 2.0);
    r.y -= (int)std::round(dy / 2.0);
    r.width  += dx;
    r.height += dy;
    
    // Remove the annotation is the size becomes negative
    if (r.width <= 0 || r.height <= 0) _annotations.pop_back();
}

void ImageAnnotator::keyPressHandler (int keypress)
{
    
    std::cout << "Keypress = " << keypress << std::endl;
    
    switch(keypress)
    {
        case 32: // 'space' (save image)
        {
            // Write annotations for this image to file
            writeAnnotationsToFile();
            writeAnnotationImages();
            
            _next_image = true;
            _image_index++;
            _annotations.clear();
        }
        case 45: // '-'
        {
            if (_annotations.empty()) break;
            
            cv::Rect& r = _annotations.back();
            adjustRectangleSize(r, -4, -4);
            redrawAnnotations();
            
            break;
        }
        case 61: // '+'
        {
            if (_annotations.empty()) break;
            
            cv::Rect& r = _annotations.back();
            adjustRectangleSize(r, +4, +4);
            redrawAnnotations();
            
            break;

        }
        case 8: // 'backspace' remove last annotation
        {
            _annotations.pop_back();
            redrawAnnotations();
            break;
        }
        case 27:  // 'esc' (undo selection)
        case 117: // 'u'   (undo selection)
        {
            _annotations.clear();
            redrawAnnotations();
            break;
        }
        case 100: // 'd' (next image)
        {
            _next_image = true;
            _image_index++;
            break;
        }
        case 113: // 'q'   (exit application)
        {
            if (_output_file.is_open()) _output_file.close();
            std::exit(EXIT_SUCCESS);
        }
            
    }
    
}

void ImageAnnotator::mouseCallBackHandler (int event, int x, int y)
{
    
    if (event ==  cv::EVENT_MBUTTONDOWN) //(cv::EVENT_LBUTTONDOWN + cv::EVENT_FLAG_SHIFTKEY))
    {
        
        for (auto& r : _annotations)
        {
            if (r.contains(cv::Point(x, y)))
            {
                adjustRectangleSize(r, -4, -4);
                redrawAnnotations();
                return;
            }
        }
        
    }
    else if (event == cv::EVENT_LBUTTONDOWN)
    {
        
        for (auto& r : _annotations)
        {
            if (r.contains(cv::Point(x, y)))
            {
                adjustRectangleSize(r, +4, +4);
                redrawAnnotations();
                return;
            }
        }
        
        cv::Rect r (x-10, y-10, 20, 20);
        _annotations.push_back(r);
        cv::rectangle(_image_with_annotations, r, cv::Scalar(0, 255, 255), 2);
        cv::imshow("Image Annotator", _image_with_annotations);
    }
    
}

void ImageAnnotator::mouseCallBackHandler (int event, int x, int y, int flags, void* userdata)
{
    ImageAnnotator* annotator = reinterpret_cast<ImageAnnotator*>(userdata);
    annotator->mouseCallBackHandler(event, x, y);
}