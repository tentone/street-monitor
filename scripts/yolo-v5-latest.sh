#!/bin/bash

echo " - Install PyTorch"
pip3 install torch torchvision torchaudio

echo " - Install YOLOv5"
git clone https://github.com/ultralytics/yolov5
cd yolov5
pip install -r requirements.txt

echo " - Download PyTorch models"
YOLO_SIZES=("n" "s" "m" "l" "x" "n6")

for i in "${YOLO_SIZES[@]}"; do
    curl -L "https://github.com/ultralytics/yolov5/releases/download/v6.1/yolov5$i.pt" -o "yolov5$i.pt"
done

echo " - Convert to ONNX"
for i in "${YOLO_SIZES[@]}"; do
    python3 export.py --weights yolov5$i.pt --include onnx
done

echo " - Copy files to models"
cp *.onnx ../../models/yolo/.

echo " - Clean up files"
cd ..
rm -rf ./yolov5

echo " - Done!"
