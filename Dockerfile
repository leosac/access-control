FROM debian:jessie

RUN apt-get update
RUN apt-get update
RUN apt-get install emacs24-nox -y
RUN apt-get install libboost-serialization-dev -y
RUN apt-get install libboost-regex-dev -y
RUN apt-get install libboost-system-dev -y
RUN apt-get install build-essential git libtool-bin -y

RUN apt-get install libtclap-dev cmake -y
RUN apt-get install autotools-dev automake pkg-config libsodium-dev -y

RUN apt-get install libgtest-dev python valgrind python-pip libpython2.7-dev -y
RUN apt-get install libcurl4-openssl-dev

RUN pip install pyzmq

RUN apt-get install python3 python3-pip -y
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.4 2
RUN pip3 install pyzmq


RUN git clone git://github.com/zeromq/libzmq.git; \
cd libzmq; \
git checkout 511d701de35377ff64d2fde9b594afc8800589ca; \
./autogen.sh; \
./configure --with-libsodium; \
make check || echo ${Red}"Warning: ZeroMQ tests failed. The build will continue but may fail."${RCol}; \
make install; \
ldconfig;


## we mount leosac source dir in read only
VOLUME /leosac_src
VOLUME /docker_scripts

ADD docker_scripts /docker_scripts


## This is way too verbose, but we don't have a choice...
## We want the necessary files to build, not the tests scripts and all.
## Otherwise, just editing a test-script would cause full rebuild ...
RUN mkdir -p /leosac_src/{cmake,cfg,docker_scripts,libmcp23s17,libpifacedigital,libzmq,scripts,spdlog,src,test,tools,zmqpp,test_helper}

ADD cfg /leosac_src/cfg/
ADD docker_scripts /leosac_src/docker_scripts/
ADD libmcp23s17 /leosac_src/libmcp23s17/
ADD libpifacedigital /leosac_src/libpifacedigital/
ADD libzmq /leosac_src/libzmq/
ADD scripts /leosac_src/scripts/
ADD spdlog /leosac_src/spdlog/
ADD test /leosac_src/test/
ADD tools /leosac_src/tools/
ADD zmqpp /leosac_src/zmqpp/
ADD json /leosac_src/json/
ADD websocketpp /leosac_src/websocketpp/
ADD cmake /leosac_src/cmake/
ADD src /leosac_src/src/
ADD CMakeLists.txt /leosac_src/

RUN /docker_scripts/build_leosac.sh


CMD [""]

ENTRYPOINT ["/docker_scripts/entry.sh"]

ADD test_helper /leosac_src/test_helper
