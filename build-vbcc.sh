#!/bin/bash
patch -d vbcc -p2 <vbcc-patch.diff
mkdir -p vbcc/bin
cd vbcc
yes '' | TARGET=z make
