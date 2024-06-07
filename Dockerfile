FROM gcc:14.1

RUN apt-get update && apt-get install -y \
    libsdl2-dev \
    ninja-build \
    python-is-python3 \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/*

# Add "unstable" packages to Debian release to allow clang-tidy-18
RUN sed -i -e 's/updates/updates unstable/' /etc/apt/sources.list.d/debian.sources
RUN apt-get update && apt-get install -y \
    clang-tidy-18 \
    cmake \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/*

WORKDIR /main
RUN curl https://raw.githubusercontent.com/llvm/llvm-project/main/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py > run-clang-tidy.py
# Invoke like so:
# python run-clang-tidy.py -clang-tidy-binary clang-tidy-18 src/**/*.cpp -p out/build/docker/

WORKDIR /main
COPY CMake* ./
COPY lib lib/
COPY src src/
COPY test test/

WORKDIR /main
RUN cmake -B /main/build --preset=docker -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build --config Release

WORKDIR /main/build
CMD ctest -C Release
# --output-on-failure
