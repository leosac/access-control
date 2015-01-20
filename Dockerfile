FROM debian:jessie

RUN apt-get update
RUN apt-get install emacs24-nox -y
RUN apt-get install libboost-serialization-dev -y
RUN apt-get install libboost-regex-dev -y
RUN apt-get install build-essential git libtool-bin -y

RUN apt-get install libtclap-dev cmake -y
RUN apt-get install autotools-dev automake pkg-config libsodium-dev -y

RUN apt-get install libgtest-dev python valgrind python-pip libpython2.7-dev -y

RUN pip install pyzmq

RUN git clone git://github.com/zeromq/libzmq.git; \
cd libzmq; \
git checkout e9b9860752ffac1a561fdb64f5f72bbfc5515b34; \
./autogen.sh; \
./configure --with-libsodium; \
make check || echo ${Red}"Warning: ZeroMQ tests failed. The build will continue but may fail."${RCol}; \
make install; \
ldconfig;


## we mount leosac source dir in read only
VOLUME /leosac_src
VOLUME /docker_scripts

ADD docker_scripts /docker_scripts


ADD . /leosac_src
RUN /docker_scripts/build_leosac.sh

CMD [""]

ENTRYPOINT ["/docker_scripts/entry.sh"]
