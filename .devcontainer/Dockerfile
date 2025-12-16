FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install language
RUN apt-get update && apt-get install -y --no-install-recommends \
  locales \
  && locale-gen en_US.UTF-8 \
  && update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 \
  && rm -rf /var/lib/apt/lists/*
ENV LANG=en_US.UTF-8

# Install timezone
RUN ln -fs /usr/share/zoneinfo/UTC /etc/localtime \
  && export DEBIAN_FRONTEND=noninteractive \
  && apt-get update \
  && apt-get install -y --no-install-recommends tzdata \
  && dpkg-reconfigure --frontend noninteractive tzdata \
  && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends \
    bash-completion \
    build-essential \
    cmake \
    gdb \
    git \
    tmux \
    tmuxp \
    pkg-config \
  && rm -rf /var/lib/apt/lists/*

COPY .devcontainer/.tmux.conf /etc/tmux.conf
RUN rosdep init || echo "rosdep already initialized"

ARG USERNAME=user
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create a non-root user
RUN groupadd --gid $USER_GID $USERNAME \
  && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
  # Add sudo support for the non-root user
  && apt-get update \
  && apt-get install -y sudo \
  && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME\
  && chmod 0440 /etc/sudoers.d/$USERNAME \
  && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends \
    libopencv-dev \
    libzmq3-dev \
    libczmq-dev \
    libmsgpack-dev \
    wget \
    curl \
    gnupg2 \
    gnupg \
    ca-certificates \
    gnupg \
    lsb-release \
    curl \
  && rm -rf /var/lib/apt/lists/*

# Add the OSRF GPG key to a new keyring directory
RUN mkdir -p /etc/apt/keyrings && \
    wget https://packages.osrfoundation.org/gazebo.gpg -O /etc/apt/keyrings/osrf-archive-keyring.gpg

# Add the Gazebo repository line, referencing the new keyring file
RUN echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/osrf-archive-keyring.gpg] http://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/gazebo-stable.list > /dev/null


# Install gazebo
RUN apt-get update && apt-get install -q -y \
    gazebo \
    libgazebo-dev \
  && rm -rf /var/lib/apt/lists/*


RUN apt-get update && apt-get install -y --no-install-recommends \
    mesa-utils \
    nvtop \
  && rm -rf /var/lib/apt/lists/*


  ENV DEBIAN_FRONTEND=

