#!/bin/bash

echo " - Update Aptitude"
apt update

echo " - Install Python 3"
apt install python3 python3-pip

echo " - Install youtube-dl"
pip3 install --upgrade yt-dlp
echo " - Download dataset"

echo "    - Highway traffic"
yt-dlp https://www.youtube.com/watch?v=nt3D26lrkho -o highway-traffic.mp4

echo "    - Highway Traffic Low Camera Shake"
yt-dlp https://www.youtube.com/watch?v=KBsqQez-O4w -o highway-traffic-shake.mp4

echo "    - Night City Traffic"
yt-dlp https://www.youtube.com/watch?v=iA0Tgng9v9U -o city-traffic-night.mp4

echo " - Done!"
