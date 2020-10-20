#!/bin/bash
echo "----- All env-vars -----"
env
echo "----- All env-vars -----"
echo
make -j4 VERBOSE=1 teditor tests
