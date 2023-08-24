#!/bin/sh
echo "Downloading required files..."
apt-get install -y wget
apt-get install -y unzip
cd /tmp
wget https://prohawkholoscanv051runtime.s3.us-west-2.amazonaws.com/phruntime.zip && unzip phruntime.zip
cp -r /tmp/phruntime/data /workspace/holoscan-sdk
rm phruntime.zip
exit 0
