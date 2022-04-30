#!/bin/bash

make .

./yolo --input "../../dataset/highway-traffic.mp4"  --model ../../models/yolo/yolov3-tiny.weights --config ../../models/yolo/yolov3-tiny.cfg