#!/bin/bash

if (( $EUID > 0 )); then
	echo " - Please run as root"
	exit
fi

echo " - Install conan"
pip3 install conan --upgrade

echo " - Done!"