#!/bin/bash

if (( $EUID > 0 )); then
	echo " - Please run as root"
	exit
fi

echo " - Update aptitude"
apt update

echo " - Install Build tools"
apt install -y cmake gcc g++ make git unzip

echo " - Install dependencies"
apt install xorg-dev libwayland-dev libxkbcommon-dev wayland-protocols extra-cmake-modules

echo "- Create libs folder"
mkdir -p ../libs
cd ../libs

echo " - Clone glfw repository"
git clone https://github.com/glfw/glfw.git

echo "    - Generate makefile"
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON .. 

echo "    - Build and Install"
make install -j8

echo " - Done!"
