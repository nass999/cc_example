#!/usr/bin/env bash

# example of the run script for running the word count

# first I'll load all my dependencies
sudo apt-get install g++-4.8

mapfile -t files < <(printf "%s\n" wc_input/*)

mkdir -p bin/Release
g++-4.8 -std=c++11 -Wall -O2 -o bin/Release/cc-example main.cpp && bin/Release/cc-example "${files[@]}"



