#### Libuuas dependency image ####
FROM ubcuas/cppuuas AS libuuasdep

#### Skypasta build image ####
FROM ubuntu:20.04 AS builder

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update -y && apt-get install -y \
	build-essential \
	cmake \
	iputils-ping \
	lsb-release \
	sudo \
	wget \
	zlib1g-dev \
	libexpat1-dev \
	libwebp-dev
RUN apt-get update -y && apt-get install -y \
	libavcodec58 \
	libavformat58 \
	libavutil56 \
	libdouble-conversion3 \
	libpcre2-16-0 \
	libswresample3 \
	libswscale5 \
	libusb-1.0-0 \
	libxcb-xinerama0 \
	libxcb-xinput0

# Hack to get logname to work
RUN rm -rf /usr/bin/logname
COPY tools/fakelogname.sh /usr/bin/logname

COPY --from=libuuasdep /usr/local/lib/ /usr/local/lib/
COPY --from=libuuasdep /usr/local/include/ /usr/local/include/

COPY spinnaker-rehost/spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz ./
RUN tar -xvf ./spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz
WORKDIR ./spinnaker-2.2.0.48-amd64
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections
RUN echo 'debconf libspinnaker/accepted-flir-eula select true' | debconf-set-selections
RUN bash -c " \
	dpkg -i libspinnaker_*.deb && \
	dpkg -i libspinnaker-dev_*.deb && \
	dpkg -i libspinnaker-c_*.deb && \
	dpkg -i libspinnaker-c-dev_*.deb && \
	dpkg -i libspinvideo_*.deb && \
	dpkg -i libspinvideo-dev_*.deb && \
	dpkg -i libspinvideo-c_*.deb && \
	dpkg -i libspinvideo-c-dev_*.deb && \
	dpkg -i spinview-qt_*.deb && \
	dpkg -i spinview-qt-dev_*.deb && \
	dpkg -i spinupdate_*.deb && \
	dpkg -i spinupdate-dev_*.deb && \
	dpkg -i spinnaker_*.deb && \
	dpkg -i spinnaker-doc_*.deb && \
	dpkg -i libgentl_*.deb \
"
# TODO: This only install the build dependencies, to actually have it work inside here more needs to be done to use the official install script

RUN mkdir -p /uas/spinnaker/lib
RUN mkdir -p /uas/spinnaker/build
WORKDIR /uas/spinnaker/lib

COPY CMakeLists.txt ./
COPY include ./include
COPY src ./src

WORKDIR /uas/spinnaker/build
RUN cmake /uas/spinnaker/lib
RUN make -j$(nproc)
