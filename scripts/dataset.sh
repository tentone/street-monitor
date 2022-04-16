#!/bin/bash

echo " - Update Aptitude"
apt update

echo " - Install Python 3"
apt install python3 python3-pip

echo " - Install youtube-dl"
pip3 install --upgrade youtube-dl

echo " - Download dataset"


echo " - Done!"
