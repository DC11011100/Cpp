# Introduction
This is basically a golden spoon-fed binary classifer/perceptron (SVM). By golden spoon feeding, I reference that the Support Vector Machine classifer is trained on, and infers with, conditioned input images. Conditioning histogram equalization and HAAR face detection. Generally faster than it's HOG counterpart, a HAAR classifier is used to initially detect human faces in general before handing off the proposed face to an SVM fueled by HOG descriptors. The latter is what's used to yes/no determine whether the suspect face is the face of interest.

While these techniques are not as accurate as more modern alternatives, this is to serve as a demonstration/commemoration of more historical methods (DSP fan here). Also wrapping this up before starting a better one with TensorFlow 

# Demo
https://www.youtube.com/watch?v=x5FgLFEIXjM&t=269s

# Facial Recognition
Faces are detected and stored using the 'c' command.
Pressing 'c' then crops, histogram equalizes, and stores a sample image into "./training_samples"
Histogram Equalization helps diminish the influence of varied light conditions.

# Research
* http://epacis.net/jcis/PDF_JCIS/JCIS11-art.0101.pdf
* https://lear.inrialpes.fr/people/triggs/pubs/Dalal-cvpr05.pdf
* https://www.cs.cmu.edu/~efros/courses/LBMV07/Papers/viola-cvpr-01.pdf

* OpenCV SVM Implementation (Sourced from https://docs.opencv.org/3.0-beta/modules/ml/doc/support_vector_machines.html):
    * Burges. A tutorial on support vector machines for pattern recognition, Knowledge Discovery and Data Mining 2(2), 1998
              (available online at http://citeseer.ist.psu.edu/burges98tutorial.html)
    * (1, 2) C.-C. Chang and C.-J. Lin. LIBSVM: a library for support vector machines, ACM Transactions on Intelligent Systems 
              and Technology, 2:27:1–27:27, 2011. (http://www.csie.ntu.edu.tw/~cjlin/papers/libsvm.pdf)


# Revision 2 Cleanup To Do (revision.cpp)
- [ ] Graphically represent conditioning and layers
- [ ] HoG sampling
     - [X] FaceIdentifier::writeImage()
     - [ ] FaceIdentifier::toHOG()
     - [ ] FaceIdentifier::sample()
        
- [ ] Face Identifier batch processing
     - [ ] FaceIdentifier::compatBatch() -- makes hog batch compatible with input layer
        
- [ ] post-trained svm --> saved.yml
     - [ ] FaceIdentifer::sessionSave()
     - [ ] FaceIdentifer::sessionLoad()

# Improvements
- [ ] Transform colorspace to YCbCr to separate light intensity. Only equalize intensity then convert back to RGB for processing
- [ ] Update existing model instead of training all at once. First investigate runtime of this, though it would allow small RAM footprint over time. 
      Could also allow a user to incrementally improve their model as more data arrives.

# Testing
- [ ] Profile accuracy with MNIST data set


# Potential improvements
- Convert sample images into eigen forms before processing. This will improve detection robustness.
- The scaling factors can be improved so that FOI (Face of Interest) can be better detected when it's smaller in the picture. This should help when testing on crowds.
- Use LBP instead of HAAR for faster input face filtering


Try to take a roughly equal number of positive and negative images to ensure non-biased SVM

# Versions
OpenCV 
C++
g++
