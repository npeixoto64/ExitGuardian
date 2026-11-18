FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    git cmake ninja-build make \
    gcc build-essential \
    python3 python3-pip \
    sdcc sdcc-doc \
    stlink-tools \
    usbutils udev \
    clang-format \
    pkg-config libusb-1.0-0-dev \
    bsdutils \
    vim-common \
    && rm -rf /var/lib/apt/lists/*

# Build stm8flash from source (not available in Ubuntu 24.04 repos)
RUN git clone https://github.com/vdudouyt/stm8flash.git /tmp/stm8flash && \
    cd /tmp/stm8flash && \
    make && \
    make install && \
    cd / && \
    rm -rf /tmp/stm8flash

# Optional: nicer prompt tools
RUN apt-get update && apt-get install -y --no-install-recommends \
    bash-completion \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
