FROM gcc:15.2

# Add "experimental" packages to Debian release to allow clang-tidy-21
RUN sed -i -e 's/updates/updates experimental/' /etc/apt/sources.list.d/debian.sources
RUN apt-get update && apt-get install -y \
    clang-tidy-21 \
    cmake \
    libsdl2-dev \
    ninja-build \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/*

WORKDIR /main
RUN curl https://raw.githubusercontent.com/llvm/llvm-project/main/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py > run-clang-tidy.py
# Invoke like so:
# python3 run-clang-tidy.py -clang-tidy-binary clang-tidy-21 src/**/*.cpp -p out/build/docker/
# (change the final directory path to the directory containing compile_commands.json)

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
