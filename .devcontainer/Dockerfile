FROM ubuntu:mantic

ENV TZ=Europe/Berlin
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    cmake-format \
    cppcheck \
    cpplint \
    curl \
    gdb \
    git \
    gnupg \
    iwyu \
    libbenchmark-dev \
    libgmock-dev \
    libgtest-dev \
    lsb-release \
    ninja-build \
    software-properties-common \
    sudo \
    vim \
    wget \
    && rm -rf /var/lib/apt/lists/*

ARG LLVM_VERSION=18
RUN wget -qO- https://apt.llvm.org/llvm.sh \
    | bash -s -- $LLVM_VERSION all \
    && rm -rf /var/lib/apt/lists/*

ARG USERNAME=ubuntu
RUN echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME
USER $USERNAME
