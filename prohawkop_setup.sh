#!/bin/sh
apt-get -y update && apt-get -y install libboost-all-dev
apt-get -y update && apt-get -y install libarchive13
apt-get -y update && apt-get -y install ffmpeg
apt-get -y update && apt-get -y install cmake cmake-curses-gui git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
apt-get -y install libcanberra-gtk-modul
apt-get install cmake cmake-curses-gui git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
apt-get install -y libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-22-dev
apt-get install -y wget
apt-get install -y unzip
mkdir -p /workspace/holoscan-sdk/include/holoscan/operators/prohawkop/
mkdir /tmp/phruntime
cd /tmp
wget https://prohawkholoscanv051runtime.s3.us-west-2.amazonaws.com/phruntime.zip && unzip phruntime.zip
cp /tmp/phruntime/*opencv*.so* /usr/local/lib
cp /tmp/phruntime/libptgde.so /usr/local/lib
cp -r /tmp/phruntime/opencv4 /usr/local/include
cp /tmp/phruntime/prohawkop.hpp /workspace/holoscan-sdk/include/holoscan/operators/prohawkop/ 
cp -r /tmp/phruntime/opencv4/opencv2 /usr/include
cp -r /tmp/phruntime/PTGDE /usr/include
cp /tmp/phruntime/PAFSFilter.bin /workspace/holoscan-sdk/include/holoscan/operators/prohawkop/
cp /workspace/holoscan-sdk/include/holoscan/operators/prohawkop/PAFSFilter.bin /workspace/holoscan-sdk/build/examples/video_replayer/cpp
cp /workspace/holoscan-sdk/include/holoscan/operators/prohawkop/PAFSFilter.bin /workspace/holoscan-sdk/build/examples/prohawk_video_replayer/cpp
#cp /tmp/phruntime/PAFSFilter.bin /workspace/holoscan-sdk/build/examples/video_replayer/cpp
rm -r /tmp/phruntime/
rm phruntime.zip*
ldconfig
exit 0
