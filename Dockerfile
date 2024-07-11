FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake git pkg-config

# Clone, build and install Geos
RUN git clone https://github.com/libgeos/geos.git && \
    cd /geos && mkdir build && cd build && \
    cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local .. && \
    make && \
    make install && \
    rm -rf /geos

ENV LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}
ENV LC_ALL=C

RUN apt-get install -y locales
RUN locale-gen en_US.UTF-8

ENV LANG=en_US.UTF-8  
ENV LANGUAGE=en_US:en  
ENV LC_ALL=en_US.UTF-8

RUN apt install -y python3-dev python3-numpy python3-pip libgl1-mesa-glx 
RUN pip install laspy scikit-image opencv-python-headless

COPY src /src
COPY third_party /third_party
COPY config/config_docker.env /config/config.env
COPY CMakeLists.txt CMakeLists.txt
COPY build-and-run.sh build-and-run.sh

CMD ./build-and-run.sh