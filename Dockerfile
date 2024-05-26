FROM gcc:14.1

RUN apt-get -y update
RUN apt-get install -y libsdl2-dev cmake ninja-build python-is-python3

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
