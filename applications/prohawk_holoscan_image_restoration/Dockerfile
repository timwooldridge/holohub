FROM nvcr.io/nvidia/clara-holoscan/holoscan:v0.5.1-dgpu
ENV DEBIAN_FRONTEND noninteractive
RUN  apt-get -y update && apt-get -y install libboost-all-dev
RUN  apt-get -y update && apt-get -y install libarchive13
RUN  apt-get -y update && apt-get -y install ffmpeg
RUN  apt-get -y update && apt-get -y install cmake cmake-curses-gui git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
RUN  apt-get install -y wget
RUN  apt-get install -y unzip
RUN mkdir /tmp/phruntime
WORKDIR /tmp
RUN wget https://prohawkholoscanv051runtime.s3.us-west-2.amazonaws.com/phruntime.zip && unzip phruntime.zip
#RUN unzip phruntime.zip
RUN mkdir -p /workspace/holoscan-sdk/include/PTGDE6.0/lib/
#RUN mkdir /opt/nvidia/holoscan/include/opencv4/
#RUN mkdir /opt/nvidia/holoscan/include/PTGDE/
#RUN mkdir /opt/nvidia/holoscan/examples/prohawk_video_replayer
RUN mkdir -p /opt/nvidia/holoscan/apps
RUN mkdir -p /opt/nvidia/holoscan/operators
#ADD phruntime /tmp/phruntime
RUN cp -r /tmp/phruntime/prohawk_video_replayer /opt/nvidia/holoscan/examples/
RUN cp -r /tmp/phruntime/opencv4 /opt/nvidia/holoscan/include/
RUN cp -r /tmp/phruntime/PTGDE /opt/nvidia/holoscan/include/
RUN cp -r /tmp/phruntime/prohawk_video_replayer /opt/nvidia/holoscan/examples/examples/
RUN cp -r /tmp/phruntime/apps/prohawk_video_replayer /opt/nvidia/holoscan/apps/
RUN cp -r /tmp/phruntime/operators/prohawkop /opt/nvidia/holoscan/operators/
RUN cp  /tmp/phruntime/*opencv*.* /usr/local/lib/
RUN cp /tmp/phruntime/libptgde.so /usr/local/lib/
#COPY phruntime/libholoscan_op_prohawkop.so /opt/nvidia/holoscan/examples/video_replayer/cpp/
RUN cp /tmp/phruntime/prohawkop.hpp /opt/nvidia/holoscan/include
RUN cp /tmp/phruntime/prohawklogo.h /opt/nvidia/holoscan/include
RUN cp /tmp/phruntime/libholoscan_op_prohawkop.so.0 /opt/nvidia/holoscan/lib/
RUN cp /tmp/phruntime/libholoscan_op_prohawkop.so.0.5.0 /opt/nvidia/holoscan/lib/
RUN cp /tmp/phruntime/PAFSFilter.bin /opt/nvidia/holoscan/examples/video_replayer/cpp/
RUN cp /tmp/phruntime/video_replayer.yaml /opt/nvidia/holoscan/examples/video_replayer/cpp/
#COPY phruntime/libholoscan_op_video_stream_replayer.so /opt/nvidia/holoscan/examples/video_replayer/cpp/
RUN cp /tmp/phruntime/libholoscan_op_video_stream_replayer.so /opt/nvidia/holoscan/lib/
RUN cp /tmp/phruntime/libholoscan_op_video_stream_replayer.so.0 /opt/nvidia/holoscan/lib/
RUN cp /tmp/phruntime/libholoscan_op_video_stream_replayer.so.0.5.1 /opt/nvidia/holoscan/lib/
RUN cp /tmp/phruntime/libholoscan_op_video_stream_replayer.so.0 /usr/local/lib/
RUN cp /tmp/phruntime/video_replayer /opt/nvidia/holoscan/examples/video_replayer/cpp/
RUN cp /tmp/phruntime/libptgde.so /usr/local/lib/
RUN cp /tmp/phruntime/libptgde.so /workspace/holoscan-sdk/include/PTGDE6.0/lib/
RUN cp /tmp/phruntime/ProhawkLogo.png /opt/nvidia/holoscan/examples/video_replayer/cpp/
RUN cp /tmp/phruntime/ld.so.conf /etc/
RUN cp /tmp/phruntime/libholoscan_op_prohawkop.so /opt/nvidia/holoscan/lib/
WORKDiR /opt/nvidia/holoscan/examples/prohawk_video_replayer/cpp
RUN rm -r /tmp/phruntime
