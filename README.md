# Visual_saliency_detection
Saliency detection application using OpenCV. This uses color, intensity and orientation features to detect salient objects in an image\\

#To run the application follow these steps:

- Change the directory path to your directory path in CMakeList.txt file.
- Do 'cmake .'
- Do 'make'
- Make sure that you give right path to input image in main.cpp before compiling.
- Execute './main' to run the binary

** You can enable debug flags in each file to display corresponding intermediate images/maps
** The last portion of the code in main.cpp is written to detect the salient parts from saliency map. This might not be the right piece of code to search for salient objects. Also this uses some hardcoded thresholds which may not work for all images. If this method is to be used, then dynamic threshold detection is needed. However the current code may draw nice countours if you are lucky.. !!! Good luck.. :)

