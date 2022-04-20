#!/bin/bash

if (( $EUID > 0 )); then
	echo " - Please run as root"
	exit
fi

echo " - Update aptitude"
apt update

echo " - Install Build tools"
apt install -y cmake gcc g++ make git unzip


echo " - Done!"
