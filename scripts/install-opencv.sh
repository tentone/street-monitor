#!/bin/bash

if (( $EUID > 0 )); then
	echo " - Please run as root"
	exit
fi

# Branch used for opencv
CHANNEL='4.5'

# Create temporary folder for compilation
echo "- Create libs folder"
mkdir -p ../libs
cd ../libs

# Install OpenCV from source
echo " - Installing Build tools"
apt install -y cmake gcc g++ make git unzip

echo " - Installing Dependencies"
apt install -y libavcodec-dev libavformat-dev libswscale-dev libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev libgtk-3-dev libpng-dev libjpeg-dev libopenexr-dev libtiff-dev libwebp-dev

# Clone the repository
echo " - Clone OpenCV repository"
git clone https://github.com/opencv/opencv.git


# Clone the repository
echo " - Clone OpenCV Contrib repository"
git clone https://github.com/opencv/opencv_contrib.git

# Checkout to version branch
cd opencv
echo " - Fetch branch $CHANNEL"
git fetch
git checkout $CHANNEL

# Building files
echo " - Build OpenCV and Install"
mkdir build
cd build
cmake ..
make install -j8