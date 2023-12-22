#!/bin/bash

mkdir build 
cd build

cmake ..
make

# Check for display :0 in the output of the w command
if w | grep -q ':0'; then # if executed via SSH display images in local rpi display
    echo "Display :0 found. Running with preserved environment."
    sudo -E ./capture_frames_for_test
else
    echo "Display :0 not found. Running without preserved environment."
    sudo ./capture_frames_for_test
fi
