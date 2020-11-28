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

SkyPasta must be run in a Linux environment (Ubuntu 20.04 is preferred)

- C++
- CMake
- exiv2-0.27.2
- Spinnaker

## Installation

### Exiv2
Run `$ sudo apt-get install exiv2`

### Spinnaker

#### Installing with Docker

Download `spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz` from FLIR Systems [here](https://meta.box.lenovo.com/v/link/view/a1995795ffba47dbbe45771477319cc3) then move the file into `skypasta/spinnaker-rehost`.

In the root of `skypasta` run `$ docker build .`

#### Installing without Docker

Download the Spinnaker SDK from FLIR Systems [here](https://meta.box.lenovo.com/v/link/view/a1995795ffba47dbbe45771477319cc3). Make sure you are installing the amd64-pkg.tar.gz file corresponding to your Linux distro version.

Extract the file you just downloaded. Then inside the terminal, go inside the folder resulting from the extraction.

For Ubuntu 20.04: run `$ sudo apt-get install libavcodec58 libavformat58 libswscale5 libswresample3 libavutil56 libusb-1.0-0 libpcre2-16-0 libdouble-conversion3 libxcb-xinput0 libxcb-xinerama0`

For Ubuntu 18.04: run `$ sudo apt-get install libavcodec57 libavformat57 libswscale4 libswresample2 libavutil55 libusb-1.0-0`

After all dependencies are installed, run `$ sudo sh install_spinnaker.sh`. For more details, refer to the README inside of the extracted Spinnaker SDK.

Then, in the root of `skypasta`, run:
```
$ mkdir build && cd build
$ cmake ..
```

## Camera Set-Up

Nothing here yet.

## Usage

Go inside `skypasta/build` (if you do not have the `build` directory, make sure you have followed all steps in [Installation](#Installation)).
Run:
```
$ make
$ ./skypasta
```

## Troubleshooting

Installing zlib: `$ sudo apt-get install zlib1-dev`

Installing expat: `$ sudo apt-get install libexpat1-dev`
