#!/bin/bash

if (( $EUID > 0 )); then
	echo " - Please run as root to install dependencies"
else
    echo " - Update Aptitude"
    apt update

    echo " - Install Python 3"
    apt install python3 python3-pip

    echo " - Install youtube-dl"
    pip3 install --upgrade yt-dlp
fi

echo " - Download dataset"
echo "    - Highway traffic"
yt-dlp https://www.youtube.com/watch?v=nt3D26lrkho -o ../dataset/highway-traffic.mp4

echo "    - Highway Traffic Low Camera Shake"
yt-dlp https://www.youtube.com/watch?v=KBsqQez-O4w -o ../dataset/highway-traffic-shake.mp4

echo "    - Street Traffic Curve"
yt-dlp https://www.youtube.com/watch?v=MNn9qKG2UFI -o ../dataset/street-traffic-curve.mp4

echo "    - Roundabout Traffic"
yt-dlp https://www.youtube.com/watch?v=FR5l48_h5Eo -o ../dataset/roundabout-traffic.mp4

echo "    - Night City Traffic"
yt-dlp https://www.youtube.com/watch?v=iA0Tgng9v9U -o ../dataset/city-traffic-night.mp4

echo " - Done!"
