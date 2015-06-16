#!/usr/bin/env bash

# first I'll load all my dependencies
apt-get install g++-4.8

# then I get a list of input files
mapfile -t files < <(printf "%s\n" wc_input/*)

# then I make the compiler output directory
mkdir -p bin/Release

# then I make the output file directory
mkdir -p wc_output

# then I build and run it
g++-4.8 -std=c++11 -Wall -O2 -o bin/Release/cc-example main.cpp && bin/Release/cc-example "${files[@]}"



