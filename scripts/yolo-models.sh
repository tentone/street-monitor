#!/bin/bash

echo " - Download YOLO V5"
cd ../dataset/yolo/

echo "    - YOLO V5 Medium"
curl https://github.com/spmallick/learnopencv/raw/master/Object-Detection-using-YOLOv5-and-OpenCV-DNN-in-CPP-and-Python/models/yolov5m.onnx -o yolov5m.onnx

echo "    - YOLO V5 Small"
curl https://github.com/spmallick/learnopencv/raw/master/Object-Detection-using-YOLOv5-and-OpenCV-DNN-in-CPP-and-Python/models/yolov5s.onnx -o yolov5s.onnx

echo "    - YOLO V5 Nano"
curl https://github.com/spmallick/learnopencv/raw/master/Object-Detection-using-YOLOv5-and-OpenCV-DNN-in-CPP-and-Python/models/yolov5n.onnx -o yolov5n.onnx

echo " - Done!"
