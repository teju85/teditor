#!/bin/bash
echo "----- All env-vars -----"
env
echo "----- All env-vars -----"
echo
make -j4 \
     CURL_OPTS=-k \
     VERBOSE=1 \
     teditor tests
