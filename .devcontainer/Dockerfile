# syntax=docker/dockerfile:1

FROM ubuntu:noble@sha256:74f92a6b3589aa5cac6028719aaac83de4037bad4371ae79ba362834389035aa

LABEL org.opencontainers.image.authors="Martin Stump <11492152+globberwops@users.noreply.github.com>"
LABEL org.opencontainers.image.documentation="https://github.com/globberwops/template-cpp"
LABEL org.opencontainers.image.licenses="BSL-1.0"
LABEL org.opencontainers.image.source="https://github.com/globberwops/template-cpp"
LABEL org.opencontainers.image.url="https://github.com/globberwops/template-cpp"
LABEL org.opencontainers.image.vendor="Martin Stump"
LABEL org.opencontainers.image.version="0.1.0"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update && apt-get install -y \
  build-essential \
  ccache \
  clang-format-18 \
  clang-tidy-18 \
  clangd-18 \
  cmake \
  cmake-format \
  cppcheck \
  cpplint \
  doxygen \
  g++-13 \
  gcc-13 \
  gdb \
  git \
  graphviz \
  iwyu \
  libc++-dev \
  lld-18 \
  lldb-18 \
  locales \
  mold \
  ninja-build \
  sudo \
  zsh

RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && \
  locale-gen
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

ENV TZ="Europe/Berlin"

ARG DEFAULT_USER=ubuntu
RUN usermod -s /usr/bin/zsh $DEFAULT_USER \
  && usermod -aG sudo $DEFAULT_USER \
  && echo "$DEFAULT_USER ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
