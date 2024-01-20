FROM seahorn/seahorn-llvm14:nightly

ENV SEAHORN=/home/usea/seahorn/bin/sea PATH="$PATH:/home/usea/seahorn/bin"

## install required pacakges
USER root

## Install latest cmake
RUN apt -y remove --purge cmake
RUN apt -y update
RUN apt -y install wget python3-pip
RUN python3 -m pip install --upgrade pip
RUN pip3 install cmake --upgrade

## import seamock
USER usea
WORKDIR /home/usea
#
## assume we are run inside c-rust 
RUN mkdir seamock
COPY --chown=usea:usea . seamock

#
WORKDIR /home/usea/seamock
#
RUN rm -Rf build && mkdir build && cd build && cmake -DCMAKE_C_COMPILER=clang-14 -DCMAKE_CXX_COMPILER=clang++-14 -DSEAHORN_ROOT=/home/usea/seahorn  -DSEA_LINK=llvm-link-14 ../ -GNinja && cmake --build .

#
### set default user and wait for someone to login and start running verification tasks
USER usea
