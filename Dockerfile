# Fedora has a small footprint and the latest package versions
ARG FEDORA_VERSION=latest
FROM fedora:${FEDORA_VERSION} as base

# Install base packages used by all stages
RUN dnf install -y \
    clang-tools-extra \
    cmake \
    gcc-c++ \
    ninja-build \
    SDL2-devel \
    && dnf clean all

# Development stage
FROM base as development
RUN dnf install -y \
    git \
    procps-ng \
    gdb \
    && dnf clean all

# Retain bash history for the dev container
# See https://code.visualstudio.com/remote/advancedcontainers/persist-bash-history
RUN SNIPPET="export PROMPT_COMMAND='history -a' && export HISTFILE=/commandhistory/.bash_history" \
    && echo "$SNIPPET" >> "/root/.bashrc"

# Build stage
FROM base as build
WORKDIR /main
COPY CMake* ./
COPY lib lib/
COPY src src/
COPY test test/

WORKDIR /main
RUN cmake -B /main/build --preset=docker -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build --config Release --parallel

WORKDIR /main/build
CMD ctest -C Release
# --output-on-failure

# For interactive ability:
# CMD [ "/bin/bash" ]
