# Street Monitor
 - Keeps track of pedestrians and vehicles in the road.
 - Segment zones of the image where vehicles and pedestrians move.

### Gettings started

- Download OpenCV 4.2.0 and install it on your system (use scripts/install-opencv.sh).
    - For optimal performance ensure that optimizations are enabled.
    - If a NVIDIA is used ensure that the CUDA SDK is installed. 
- Install CMAKE and setup your IDE for C++ development.
- Install GCC for linux development or Visual Studio for windows development.
- Dependencies can also be obtained from the conan package manager (https://conan.io/center/)
    - To install dependencies run `conan install .`.


### Dataset
 - Data for testing can be downloaded from youtube.
 - The file scripts/dataset.sh can be used to obtain test data.
 - Test video data is obtained from youtube might be suject to licensing and not available for commercial usage.

### YOLO V5 Model
 - YOLO model is used to classify moving objects such as cars and pedestrians.
 - YOLO V5 is available on https://pytorch.org/hub/ultralytics_yolov5/ / https://github.com/ultralytics/yolov5, check the latest releases on github.
    - The PyTorch models have to be converted into ONNX files.

### License
- This project is distributed under MIT license and can be freely used for commercial applications.
- Project license is available on the Github page.
