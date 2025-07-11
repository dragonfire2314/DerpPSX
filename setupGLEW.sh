#!/bin/bash

wget https://gigenet.dl.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0.zip
unzip glew-2.1.0.zip
rm glew-2.1.0.zip

cd glew-2.1.0

make
sudo make install
make clean