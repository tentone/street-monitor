#!/bin/bash

echo " - Install PyTorch"
pip3 install torch torchvision torchaudio

echo " - Install YOLOv5"
git clone https://github.com/ultralytics/yolov5
cd yolov5
pip install -r requirements.txt

echo " - Download PyTorch models"
curl https://github.com/ultralytics/yolov5/releases/download/v6.1/yolov5n.pt -o yolov5n.pt
# curl https://github.com/ultralytics/yolov5/releases/download/v6.1/yolov5m.pt -o yolov5m.pt
# curl https://github.com/ultralytics/yolov5/releases/download/v6.1/yolov5l.pt -o yolov5l.pt
# curl https://github.com/ultralytics/yolov5/releases/download/v6.1/yolov5x.pt -o yolov5x.pt
# curl https://github.com/ultralytics/yolov5/releases/download/v6.1/yolov5n6.pt -o yolov5n6.pt

echo " - Convert to ONNX"
python3 export.py --weights yolov5n.pt --include torchscript onnx

# echo " - Clean up files"
# rm -rf ./yolov5