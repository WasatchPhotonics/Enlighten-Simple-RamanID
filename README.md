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

You can therefore build this project on Windows by double-clicking the .sln file
and compiling under Visual Studio.

# Testing

The included sample data and command-line options allow quick testing from a
command shell:

    $ bin/identify --help
    bin/identify Handheld 1.0 (C) 2021, Wasatch Photonics

    Usage: bin/identify [--brief] [--verbose] [--alt] [--unknown] [--thresh frac] [--streaming] --library /path/to/library [sample.csv...]
           bin/identify --help

    NOTE:  This version has been modified from the original in the following key respects:

    - Only one library compound is supported
    - All math is single-precision float
    - Memory is static rather than from heap
    - Other optimizations as necessary to reduce memory usage and runtime

    Example: bin/identify --library libraries/WP-785 data/WP-785/*.csv

    Options:
        --alt       output match 'alternates'
        --brief     output results on one line per sample
        --streaming read streaming spectra from stdin
        --thresh    unknown threshold (default 0.95)
        --unknown   output unknown spectral residue
        --verbose   include debugging output
        --auth      authenticate only

Example:

    $ bin/identify --library libraries/WP-785 data/WP-785/*.csv 
    Interim: sample 2mmHDPE:            matched library 2mmHDPE
    Interim: sample 4mmHDPE:            matched library 2mmHDPE
    Interim: sample Acetone:            matched library Acetone
    Interim: sample Acetone2mmHDPE:     matched library 2mmHDPE
    Interim: sample Acetone4mmHDPE:     matched library 2mmHDPE
    Interim: sample Aklkanes2mmHDPE:    matched library 2mmHDPE
    Interim: sample Alkanes:            matched library Alkanes
    Interim: sample Alkanes4mmHDPE:     matched library 2mmHDPE
    Interim: sample Cyclohexane:        matched library Cyclohexane
    Interim: sample Cyclohexane2mmHDPE: matched library Cyclohexane
    Interim: sample Cyclohexane4mmHDPE: matched library 2mmHDPE
    Interim: sample Ethanol:            matched library Ethanol
    Interim: sample Ethanol2mmHDPE:     matched library HDPE-20190905
    Interim: sample Ethanol4mmHDPE:     matched library HDPE-20190905
    Interim: sample Ibuprofen:          matched library ibuprofen-20190904-085911-272806-WP-00340
    Interim: sample Ibuprofen2mmHDPE:   matched library 2mmHDPE
    Interim: sample Ibuprofen4mmHDPE:   matched library HDPE-20190905
    Interim: sample Isopropanol:        matched library Isopropanol
    Interim: sample Isopropanol2mmHDPE: matched library 2mmHDPE
    Interim: sample Isopropanol4mmHDPE: matched library 2mmHDPE


# Backlog

- cache interpolated library between streamed spectra, only re-interpolate if 
  length or first/last wavenumber change
- C cleanup
    - initialize all variables
    - free() memory

# History

- see [CHANGELOG](README_CHANGELOG.md)
