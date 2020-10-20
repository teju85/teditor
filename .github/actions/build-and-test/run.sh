#!/bin/bash
echo "----- All env-vars -----"
env
echo "----- All env-vars -----"
echo
make -j4 teditor tests
