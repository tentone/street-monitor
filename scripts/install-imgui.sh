#!/bin/bash

if (( $EUID > 0 )); then
	echo " - Please run as root"
	exit
fi



echo " - Done!"