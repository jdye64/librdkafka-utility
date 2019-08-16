#!/usr/bin/env bash

echo "Activating Conda kafka_dev environment"
source activate kafka_dev

INSTALL_PREFIX=${INSTALL_PREFIX:=${PREFIX:=${CONDA_PREFIX}}}

mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}

make -j
make install