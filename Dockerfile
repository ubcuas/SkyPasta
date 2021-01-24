#### Libuuas dependency image ####
FROM ubcuas/cppuuas AS libuuasdep


#### Cmake dependency image ####
FROM ubuntu:16.04 AS cmakedep

RUN mkdir -p /cmakebuild
WORKDIR /cmakebuild

RUN apt-get update -y && apt-get install -y build-essential wget libssl-dev

ARG CMAKE_VERSION=3.18.2
RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}.tar.gz
RUN tar -xzf cmake-${CMAKE_VERSION}.tar.gz

WORKDIR /cmakebuild/cmake-${CMAKE_VERSION}
RUN ./bootstrap
RUN make -j4


#### Skypasta build image ####
FROM ubuntu:16.04 AS builder

RUN apt-get update -y && apt-get install -y build-essential libssl-dev
ARG CMAKE_VERSION=3.18.2
RUN mkdir -p /cmakebuild/cmake-${CMAKE_VERSION}
WORKDIR /cmakebuild/cmake-${CMAKE_VERSION}
COPY --from=cmakedep /cmakebuild/cmake-${CMAKE_VERSION}/ ./
RUN make install
WORKDIR /

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update -y && apt-get install -y \
	iputils-ping \
	lsb-release \
	sudo \
	wget \
	zlib1g-dev \
	libexpat1-dev \
	libwebp-dev
RUN apt-get update -y && apt-get install -y \
	libpcre2-16-0 \
	libusb-1.0-0 \
	libxcb-xinerama0

# Hack to get logname to work
RUN rm -rf /usr/bin/logname
COPY tools/fakelogname.sh /usr/bin/logname

COPY --from=libuuasdep /usr/local/lib/ /usr/local/lib/
COPY --from=libuuasdep /usr/local/include/ /usr/local/include/

COPY spinnaker-rehost/spinnaker-2.0.0.147-Ubuntu16.04-amd64-pkg.tar.gz ./
RUN tar -xvf ./spinnaker-2.0.0.147-Ubuntu16.04-amd64-pkg.tar.gz
WORKDIR ./spinnaker-2.0.0.147-amd64
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
	dpkg -i spinnaker-doc_*.deb \
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
