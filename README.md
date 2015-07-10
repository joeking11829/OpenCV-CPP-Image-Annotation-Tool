# Image Annotation Tool

This is a small tool for annotating objects in images and saving bounding boxes as CSV file and optionally the image patches as PNG. Useful for extracting train/test data from visual data to use for machine learning and computer vision applications.

![annotations](https://cloud.githubusercontent.com/assets/5536129/8628157/dd4ee170-2750-11e5-9312-a784616a7529.png)

## Dependencies
- `OpenCV 3.0.0`
- `Boost 1.56.0`

## Commandline Arguments

* `--inputdir` [path with JPG/PNG/BMP files] 
* `--csvfile` [path to CSV for saving annotations] 
* `--outputdir` [optional output path for saving patches]

## Usage

* Click `left mouse button` to add annotation
* Click again to increase size of annotation (also `+/-` keys can be used)
* Press `BACKSPACE` to undo last annotation
* Press `U` to remove all annotations in current image
* Press `SPACE` to save all annotations and continue to next image
* Press `ESC` to close the application