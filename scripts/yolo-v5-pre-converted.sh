#!/bin/bash

echo " - Download Pre-Converted YOLO V5"
cd ../models
mkdir yolo
cd ./yolo/

echo "    - YOLO V5 Medium"
curl https://raw.githubusercontent.com/spmallick/learnopencv/master/Object-Detection-using-YOLOv5-and-OpenCV-DNN-in-CPP-and-Python/models/yolov5m.onnx -o yolov5m.onnx

echo "    - YOLO V5 Small"
curl https://raw.githubusercontent.com/spmallick/learnopencv/master/Object-Detection-using-YOLOv5-and-OpenCV-DNN-in-CPP-and-Python/models/yolov5s.onnx -o yolov5s.onnx

echo "    - YOLO V5 Nano"
curl https://raw.githubusercontent.com/spmallick/learnopencv/master/Object-Detection-using-YOLOv5-and-OpenCV-DNN-in-CPP-and-Python/models/yolov5n.onnx -o yolov5n.onnx

echo "    - YOLO V5 Classes"
curl https://raw.githubusercontent.com/spmallick/learnopencv/master/Object-Detection-using-YOLOv5-and-OpenCV-DNN-in-CPP-and-Python/coco.names -o yolo.names

echo " - Done!"
