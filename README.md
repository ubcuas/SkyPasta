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

- Docker

**Local:**

- C++
- CMake
- Spinnaker 2.2.0.48 from FLIR Systems

## Setup

### Installing Dependencies

**For usage with Docker:**

Run `$ git submodule update --init --recursive` to fetch the Spinnaker packages from [spinnaker-rehost](https://gitlab.com/ubcuas/spinnaker-rehost/-/tree/43b03e4437f86cd919c6eb5ab4fac07eefca06b8).

If you received `Please make sure you have the correct access rights and the repository exists.` error, instead download `spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz` and `spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz` from [here](https://meta.box.lenovo.com/v/link/view/a1995795ffba47dbbe45771477319cc3) then move the files into `skypasta/spinnaker-rehost`.

**For usage without Docker:**

Download `spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz` from [here](https://meta.box.lenovo.com/v/link/view/a1995795ffba47dbbe45771477319cc3) and extract `spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz`.

Go to the root of the folder of the extracted folder (it should be named `spinnaker-2.2.0.48-amd64`) and run `$ sudo apt-get install libavcodec58 libavformat58 libswscale5 libswresample3 libavutil56 libusb-1.0-0 libpcre2-16-0 libdouble-conversion3 libxcb-xinput0 libxcb-xinerama0`

Then run `$ sudo sh install_spinnaker.sh`

For more information, refer to the `README.txt` inside of `spinnaker-2.2.0.48-amd64`.

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
Where `<camera_device>` is the port your camera is running on.

**Without Docker**: Go inside `skypasta/build` (if you do not have the `build` directory, make sure you have followed all steps in [Setup](#Setup)).

Run:

```bash
make
./skypasta
```

## Troubleshooting

---

```bash
$ docker build .
COPY failed: stat /var/lib/docker/tmp/docker-builder912847276/spinnaker-rehost/spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz: no such file or directory
```

Make sure that `spinnaker-2.2.0.48-Ubuntu20.04-amd64-pkg.tar.gz` is inside of `skypasta/spinnaker-rehost`. If not, make sure you have followed the instructions in [Setup](Setup).

---

Installing zlib:
`$ sudo apt-get install zlib1-dev`

---

Installing expat:
`$ sudo apt-get install libexpat1-dev`
