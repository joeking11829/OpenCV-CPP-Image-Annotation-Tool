//
//  main.cpp
//  PatchExtractionTool
//
//  Created by Tom Runia on 04/03/15.
//  Copyright (c) 2015 TomTom. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>

#include <boost/program_options.hpp>

#include "ImageAnnotator.h"

std::string inputdir;
std::string outputdir;
std::string csvfile;

bool parseCommandLineParams (int argc, const char * argv[])
{
    
    try
    {
        boost::program_options::options_description description("Allowed options");
        
        description.add_options()
        ("inputdir,i", boost::program_options::value<std::string>(), "Input directory containing images (png, jpg or bmp)")
        ("outputdir,p",boost::program_options::value<std::string>(), "Output directory for writing image bounding boxes as image.")
        ("csvfile,d",  boost::program_options::value<std::string>(), "CSV file for writing annotations.");
        
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(description).run(), vm);
        boost::program_options::notify(vm);
        
        if (!vm.count("inputdir"))
        {
            std::cout << "Application needs input option." << std::endl;
            std::cout << "Please specify --input..." << std::endl;
            return false;
        }
        else
        {
            inputdir = vm["inputdir"].as<std::string>();
        }
        
        if (!vm.count("outputdir") && !vm.count("csvfile"))
        {
            std::cout << "Application needs output option." << std::endl;
            std::cout << "Either specify --patchDir or --dataFile..." << std::endl;
            return false;
        }
        
        if (vm.count("outputdir"))
        {
            outputdir = vm["outputdir"].as<std::string>();
        }
        
        if (vm.count("csvfile"))
        {
            csvfile = vm["csvfile"].as<std::string>();
        }
        
    }
    catch (std::exception& e)
    {
        throw std::runtime_error("An error occured while parsing command line arguments...");
    }
    
    return true;
    
}

int main(int argc, const char * argv[])
{
    
    bool status = parseCommandLineParams(argc, argv);
    if (!status) return EXIT_FAILURE;
    
    ImageAnnotator annotator;
    annotator.labelImagesInDirectory(inputdir, csvfile, outputdir);
    
}

