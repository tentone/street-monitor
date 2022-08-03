#!/bin/bash

cd ..

rm ./speed-camera

cmake .
make -j4

./speed-camera "./dataset/city-traffic-night.mp4"