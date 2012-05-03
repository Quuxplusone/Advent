#!/bin/bash
patch -d vbcc -p1 <vbcc-patch.diff
mkdir -p vbcc/bin
cd vbcc
TARGET=z make < /dev/null
