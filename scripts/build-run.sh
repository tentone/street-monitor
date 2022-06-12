#!/bin/bash

cd ..

rm ./speed-camera

cmake .
make -j4

./speed-camera "./dataset/roundabout-traffic.mp4"