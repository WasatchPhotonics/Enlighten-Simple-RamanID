# Overview

This provides an algorithm to identify spectra

# Contents

- src/
    - Files for running identification

- data/
    - Sample test spectra to analyze with the algorithm.

- libraries/
    - Spectral libraries of "pure" compounds.  Each library is in its own 
      directory, and contains CSV files representing pure spectra of the named 
      compound. 
    - WP-785/
        - taken from WP-00340
    - SiG-785/
        - taken from WP-00438

# Build

## MacOS / Linux

    $ make
    $ bin/identify --help

## Windows

I was able to build and run from MinGW with only a couple tweaks, but wasn't sure
how to deploy a portable statically linked binary.  Therefore I added a Visual 
C++ project, and currently build on Windows using Visual Studio Community Edition.

# Backlog

- cache interpolated library between streamed spectra, only re-interpolate if 
  length or first/last wavenumber change
- C cleanup
    - initialize all variables
    - free() memory

# History
- 2022-02-10 1.0.0
    - initial import
