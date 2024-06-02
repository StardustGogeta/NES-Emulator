FROM gcc:14.1

RUN apt-get update && apt-get install -y \
    cmake \
    libsdl2-dev \
    ninja-build \
    python-is-python3 \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/*

WORKDIR /main
COPY CMake* ./
COPY lib lib/
COPY src src/
COPY test test/

WORKDIR /main
RUN cmake -B /main/build --preset=docker
RUN cmake --build build --config Release

WORKDIR /main/build
CMD ctest -C Release
# --output-on-failure
