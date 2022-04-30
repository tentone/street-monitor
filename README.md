# Speed Radar
 - Speed radar system build as an weekend project.
 - Detect cars in the road, reads their license plate and estimates their velocity.

### Gettings started

- Download OpenCV 4.2.0 and install it on your system (use scripts/install-opencv.sh).
- Install CMAKE and setup your IDE for C++ development.
- Install GCC for linux development or Visual Studio for windows development.
- Dependencies can also be obtained from the conan package manager (https://conan.io/center/)
    - To install dependencies run `conan install .`.


### Dataset
 - Data for testing can be downloaded from youtube.
 - The file scripts/dataset.sh can be used to obtain test data.
 - Data might be suject to licensing and not available for commercial usage, use at your own risk.

### Models
 - Pre-trained yolo V3 model can be obtained from https://pjreddie.com/darknet/yolo.
 - The YOLO V3 tiny model offers the best performance and is capable of detecting the required categories.

### License
- This project is distributed under MIT license and can be freely used for commercial applications.
- Project license is available on the Github page.

