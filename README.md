# SFND 2D Feature Tracking

<img src="images/keypoints.png" width="820" height="248" />

The idea of the camera course is to build a collision detection system - that's the overall goal for the Final Project. As a preparation for this, you will now build the feature tracking part and test various detector / descriptor combinations to see which ones perform best. This mid-term project consists of four parts:

* First, you will focus on loading images, setting up data structures and putting everything into a ring buffer to optimize memory load. 
* Then, you will integrate several keypoint detectors such as HARRIS, FAST, BRISK and SIFT and compare them with regard to number of keypoints and speed. 
* In the next part, you will then focus on descriptor extraction and matching using brute force and also the FLANN approach we discussed in the previous lesson. 
* In the last part, once the code framework is complete, you will test the various algorithms in different combinations and compare them with regard to some performance measures. 

See the classroom instruction and code comments for more details on each of these parts. Once you are finished with this project, the keypoint matching part will be set up and you can proceed to the next lesson, where the focus is on integrating Lidar points and on object detection using deep-learning. 

## Dependencies for Running Locally
1. cmake >= 2.8
 * All OSes: [click here for installation instructions](https://cmake.org/install/)

2. make >= 4.1 (Linux, Mac), 3.81 (Windows)
 * Linux: make is installed by default on most Linux distros
 * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
 * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)

3. OpenCV >= 4.1
 * All OSes: refer to the [official instructions](https://docs.opencv.org/master/df/d65/tutorial_table_of_content_introduction.html)
 * This must be compiled from source using the `-D OPENCV_ENABLE_NONFREE=ON` cmake flag for testing the SIFT and SURF detectors. If using [homebrew](https://brew.sh/): `$> brew install --build-from-source opencv` will install required dependencies and compile opencv with the `opencv_contrib` module by default (no need to set `-DOPENCV_ENABLE_NONFREE=ON` manually). 
 * The OpenCV 4.1.0 source code can be found [here](https://github.com/opencv/opencv/tree/4.1.0)

4. gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using either [MinGW-w64](http://mingw-w64.org/doku.php/start) or [Microsoft's VCPKG, a C++ package manager](https://docs.microsoft.com/en-us/cpp/build/install-vcpkg?view=msvc-160&tabs=windows). VCPKG maintains its own binary distributions of OpenCV and many other packages. To see what packages are available, type `vcpkg search` at the command prompt. For example, once you've _VCPKG_ installed, you can install _OpenCV 4.1_ with the command:
```bash
c:\vcpkg> vcpkg install opencv4[nonfree,contrib]:x64-windows
```
Then, add *C:\vcpkg\installed\x64-windows\bin* and *C:\vcpkg\installed\x64-windows\debug\bin* to your user's _PATH_ variable. Also, set the _CMake Toolchain File_ to *c:\vcpkg\scripts\buildsystems\vcpkg.cmake*.


## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./2D_feature_tracking`.


## Midterm

1. Number of keypoints

| Detector | SHITOMASI | HARRIS | FAST | BRISK | ORB | AKAZE | SIFT | 
| :---:    | :---:  | :---:  | :---:  |  :---: | :---:  | :---:  | :---:  | 
| image0 | 122 | 17 | 112 | 201 | 85 | 156 | 130 | 
| image1 | 117 | 14 | 109 | 207 | 100 | 149 | 127 | 
| image2 | 122 | 18 | 116 | 214 | 104 | 152 | 118 | 
| image3 | 117 | 20 | 115 | 204 | 110 | 146 | 133 | 
| image4 | 115 | 25 | 111 | 214 | 106 | 157 | 133 | 
| image5 | 112 | 39 | 115 | 212 | 121 | 160 | 135 | 
| image6 | 112 | 17 | 107 | 213 | 127 | 168 | 132 | 
| image7 | 120 | 28 | 106 | 196 | 117 | 168 | 141 | 
| image8 | 111 | 25 | 110 | 197 | 116 | 168 | 150 | 
| image9 | 108 | 31 | 99 | 174 | 116 | 165 | 131 | 

2. Number of matched keypoints

| Detector, Descriptor | BRISK | BRIEF |   ORB  |  FREAK | AKAZE | SIFT  | 
| :---:               | :---:  | :---: | :---:  |  :---: | :---: | :---: | 
| SHITOMASI           | 680    | 804    | 755    | 566    | X    | 910   |
| HARRIS              | 131    | 164    | 165    | 131    | X    | 170   |
| FAST                | 609    | 698    | 661    | 506    | X    | 807   |
| BRISK               | 1017   | 1045   | 741    | 859    | X    | 1282  |
| ORB                 | 629    | 446    | 495    | 334    | X    | 733   |
| AKAZE               | 1060   | 1065   | 885    | 924    | 1117 | 1219  |
| SIFT                | 523    | 605    | X      | 500    | X    | 803   |

3. runtime

| Detector, Descriptor | BRISK | BRIEF |   ORB  |  FREAK | AKAZE | SIFT  | 
| :---:               | :---:  | :---: | :---:  |  :---: | :---: | :---: | 
| SHITOMASI           | 6.4    | 2.4    | 3.0    | 9.0    | X    | 8.2   |
| HARRIS              | 6.7    | 2.6    | 3.3    | 9.2    | X    | 8.4   |
| FAST                | 4.5    | 0.5    | 1.2    | 7.1    | X    | 6.8   |
| BRISK               | 10.4   | 6.4    | 8.4    | 13.0   | X    | 15.8  |
| ORB                 | 5.5    | 1.5    | 3.8    | 8.2    | X    | 12.1   |
| AKAZE               | 14.3   | 10.1   | 11.8   | 16.9   | 20.0 | 18.2   |
| SIFT                | 34.9   | 30.9   | X      | 37.7   | X    | 60.8   |

4. TOP3 detector/descriptor combination
- BRISK/BRIEF(1045, 6.4s), FAST/SIFT (807, 6.8s), SHITOMASI/BRIEF(804, 2.4s)
- I chose the case where there are many matched numbers in less than 10 seconds.
