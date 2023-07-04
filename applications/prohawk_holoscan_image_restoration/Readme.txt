
Setup Guide
ProHawk Application for NVIDIA Holoscan 0.5.1


1 Introduction

This Quick Start Guide provides an overview on how to setup and configure the
ProHawk Application and Operator for NVIDIA Holoscan 0.5.0. We have tried to make this overview
clear, easy to understand, and informative. We value the relationship that we
have with our users and believe this guide with be valuable in expediting the
setup. This guide covers the v0.5.1 release of the Holoscan/ProHawk Application/Operator.

The ProHawk Holoscan Application/Operator receives video from a previously recorded file via Holoscan's video_stream_replayer operator, and then works to enhance the imagery so that
additional details may be seen in the video output.  The ProHawk Holoscan applicatio/operator provides various preset and detailed enhancement parameters that can be
changed by the command menu display.



2 Running the ProHawk Holoscan Application/Operator:

To run the ProHawk Application/Operator for NVIDIA Holoscan, first build the Docker image using the provided DockerFile: e.g:
a) sudo docker build -t <ex: prohawkop_holoscansdk:v051_test> .
b) sudo docker run -it --rm --net host --runtime=nvidia -v /tmp/.X11-unix:/tmp/.X11-unix -v $nvidia_icd_json:$nvidia_icd_json:ro -e NVIDIA_DRIVER_CAPABILITIES=graphics,video,compute,utility,display -e DISPLAY=$DISPLAY <newly create imgage>
c) cd holohub/applications/prohawk_holoscan_image_restoration 
d) ./prohawk_video_replayer

After building the docker image and running/creating the docker container, the ProHawk Holoscan application and operator will be fully
installed and ready for use.

In the above example, be sure to replace <ex: prohawkop_holoscansdk:v051_test> with your docker image name

For technical support or other assistance, please don't hesitate to
visit us at https://prohawk.ai/contact/
