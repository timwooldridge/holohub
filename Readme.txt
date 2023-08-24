1. Introduction

This Quick Start Guide provides an overview on how to setup and configure the
ProHawk Application and Operator for NVIDIA Holoscan 0.5.1. We have tried to make this overview
clear, easy to understand, and informative. We value the relationship that we
have with our users and believe this guide with be valuable in expediting the
setup. This guide covers the v0.5.1 release of the Holoscan/ProHawk Application and Operator.

The ProHawk Holoscan Application/Operator receives video from a previously recorded file via Holoscan's video_stream_replayer operator, and then works to enhance the imagery so that
additional details may be seen in the video output. The ProHawk Holoscan application/operator provides various preset and detailed enhancement parameters that can be
changed by the command menu display.



2. Building the ProHawk Holoscan Application/Operator:

To run the ProHawk Application/Operator for NVIDIA Holoscan, first build the Docker image using the following procedure:
2a) sudo ./run install_gxf
2b) sudo ./run setup
2c) sudo ./run launch

(Note: After executing the launch command (above), the docker container will be created and you will be promptd in the container build directory.)

2d) ./../prohawkop_setup1.sh
2e) cmake .. (from within the docker container build directory)
2f) ./../prohawkop_setup2.sh  - You will be prompted to answer installation locale question. Answer bsed on you locale.
2g) make
2h) make install

After building the docker image and running/creating the docker container, the ProHawk Holoscan application and operator will be fully
installed and ready for use.


3. Running the ProHawk Holoscan Application/Operator:

3a) cd /workspace/holoscan-sdk/build/examples/prohawk_video_replayer/cpp
3b) ./prohawk_video_replayer - Note: Edit the prohawk_video_replayer.yaml file to point to the source video file directory. (ex: /workspace/holoscan-sdk/data/endoscopy/video)

Note: To change the filter preset, click on the command menu display window and select the desired preset (e.g. "0" for Auto Filter Select (AFS), "1" for Lowlight, etc..)



For technical support or other assistance, please don't hesitate to
visit us at https://prohawk.ai/contact/
