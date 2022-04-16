#!/bin/bash

echo " - Update Aptitude"
apt update

echo " - Install Python 3"
apt install python3 python3-pip

echo " - Install youtube-dl"
pip3 install --upgrade youtube-dl

echo " - Download dataset"

echo "    - Highway traffic"
youtube-dl https://www.youtube.com/watch?v=nt3D26lrkho

echo "    - Highway Traffic Low Camera Shake"
youtube-dl https://www.youtube.com/watch?v=KBsqQez-O4w

echo "    - Night City Traffic"
youtube-dl https://www.youtube.com/watch?v=iA0Tgng9v9U

echo " - Done!"
