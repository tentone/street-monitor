#!/bin/bash

echo " - Install cpplint"
pip3 install cpplint

echo " - Navigate to source"
cd ../source

echo " - Lint code"
cpplint --recursive *.cpp

echo " - Done!"
