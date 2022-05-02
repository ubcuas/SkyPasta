# SkyPasta

`SkyPasta` is the software suite on the aircraft payload that takes pictures, tags them with telemetry info, and sends them to GCS.

## Connections

```
[Camera]---<USB>---[SkyPasta]---<TCP>---[Skylink]
                       |
                   <TCP/IP>
                       |
                [Golden Retriever]
```

## Dependencies

SkyPasta must be run in a Linux environment (Ubuntu 20.04).

**Docker:**

- Docker (not supported with newer versions of the library)

**Local:**

- C++
- CMake
- OpenCV
- Spinnaker 2.5.0.80 from FLIR Systems
- It is recommended to be running https://odroid.in/ubuntu_20.04lts/XU3_XU4_MC1_HC1_HC2/ubuntu-20.04-5.4-mate-odroid-xu4-20210113.img.xz

## Setup

### Installing Dependencies

**For usage with Docker:**

Run `$ git submodule update --init --recursive` to fetch the Spinnaker packages from [spinnaker-rehost](https://gitlab.com/ubcuas/spinnaker-rehost/-/tree/43b03e4437f86cd919c6eb5ab4fac07eefca06b8).

If you received `Please make sure you have the correct access rights and the repository exists.` error, instead download `spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz` and `spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz` from [here](https://meta.box.lenovo.com/v/link/view/a1995795ffba47dbbe45771477319cc3) then move the files into `skypasta/spinnaker-rehost`.

**For usage without Docker:**

Tested on an Odroid XU4 running ubuntu-20.04-5.4-mate-odroid-xu4-20210113.img.

Download `spinnaker-2.5.0.80-Ubuntu20.04-armhf-pkg.tar.gz` from [here](https://meta.box.lenovo.com/v/link/view/a1995795ffba47dbbe45771477319cc3) and extract `spinnaker-2.5.0.80-Ubuntu20.04-amd64-pkg.tar.gz`.

Go to the root of the folder of the extracted folder (it should be named `spinnaker-2.5.0.80-amd64`) and run `$ sudo apt-get install libavcodec58 libavformat58 libswscale5 libswresample3 libavutil56 libusb-1.0-0 libpcre2-16-0 libdouble-conversion3 libxcb-xinput0 libxcb-xinerama0`

Then run `$ sudo sh install_spinnaker.sh`

For more information, refer to the `README.txt` inside of `spinnaker-2.5.0.80-amd64`.

### Build

**With Docker**: in the root of `skypasta` run

```bash
docker build . -t ubcuas/skypasta
```

**Without Docker**: in the root of `skypasta`, run

```bash
mkdir build && cd build
cmake ..
```

Note: every time you rebuild the repository, make sure you are inside of `skypasta/build` and then running `$ cmake ..`.

## Camera Setup

Nothing here yet.

## Usage

**With Docker**: 

```bash
docker run --device <camera_device>:<camera_device> ubcuas/skypasta
```
Where `<camera_device>` is the location path of your camera connected to your device.

**Without Docker**: Go inside `skypasta/build` (if you do not have the `build` directory, make sure you have followed all steps in [Setup](#Setup)).

Run:

```bash
make
./skypasta
```

The images are initially saved at a folder called "Saving"

Then they are moved to a folder called "Untagged" when they are fully saved but not tagged

Then they are moved to a folder called "Tagged" when/if they are tagged with geolocation


Optinal arguments:
1. Connected camera type. Use "flir" to set to FLIR Camera, GenericUSB camera is used otherwise. Default: GenericUSB
2. Tag images. Set to "tag" to tag images with geolocation Default: Not tagging images
3. Seconds to run. Set the number of seconds the code should run for. Set to -1 to run indefinitely. Default: -1 

## Troubleshooting

---
Here is the list of commands you can run on an Odroid XU4 that has been imaged with ubuntu-20.04-5.4-mate-odroid-xu4-20210113.img to get it to run SkyPasta:

sudo apt-get update

sudo apt install git-all

git clone https://gitlab.com/ubcuas/skypasta.git

[get the spinnaker tar file]

tar -xvf spinnaker-2.5.0.80-Ubuntu20.04-armhf-pkg.tar

cd spinnaker-2.5.0.80-armhf

sudo sh install_spinnaker_arm.sh

(for newer spinnakers you might need to also run the following: sudo apt --fix-broken install)

sudo apt-get install exiv2

sudo apt-get install zlib1g-dev

sudo apt-get install libexpat1-dev (for raspbian: sudo apt-get install expat)

sudo apt-get install cmake

sudo apt install libopencv-dev python3-opencv

pkg-config --modversion opencv4

sudo mv /usr/include/opencv4/opencv2 /usr/include/

mkdir build && cd build 

cmake .. 

make 

sudo ./skypasta 

---

```bash
$ docker build .
COPY failed: stat /var/lib/docker/tmp/docker-builder912847276/spinnaker-rehost/spinnaker-2.5.0.80-Ubuntu20.04-amd64-pkg.tar.gz: no such file or directory
```

Installing zlib:
`$ sudo apt-get install zlib1-dev`

---

Installing expat:
`$ sudo apt-get install libexpat1-dev`
