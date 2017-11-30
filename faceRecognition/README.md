Accidentally pushed a buggy version, but I also need to clean up the code a bit. So, as of right now this is in beta until I have more time again to improve it.
Here's a video of the working project. I'll be migrating it to a QT app so that it's more easily used by a wider population. Until then,
https://www.youtube.com/watch?v=x5FgLFEIXjM&t=269s

# Facial Recognition
Faces are detected and stored using the 'c' command.
Pressing 'c' then crops, histogram equalizes, and stores a sample image into "./training_samples"
Histogram Equalization helps diminish the influence of varied light conditions.

# Potential improvements
- Convert sample images into eigen forms before processing. This will improve detection robustness.
- The scaling factors can be improved so that FOI (Face of Interest) can be better detected when it's smaller in the picture. This should help when testing on crowds.



Try to take a roughly equal number of positive and negative images to ensure non-biased SVM
VERSIONS
OpenCV 
C++
g++
