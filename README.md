# Overview

This is a deliberately-simple algorithm and sample library for performing
Raman identification.

It is provided as a demonstration of how an external application can be
called from Wasatch Photonics [ENLIGHTEN™](https://wasatchphotonics.com/product-category/software/)
using the ENLIGHTEN™ plug-in architecture.

In particular, it is the "external application" counterpart of ENLIGHTEN™'s
RamanID plug-in, whose source can be found here, and which is included in
the ENLIGHTEN™ distribution:

- https://github.com/WasatchPhotonics/ENLIGHTEN/tree/main/pluginExamples/Raman

Therefore, although you can test this "application" on its own from
the command-line (see [Testing](#Testing), below), it is really intended to
be installed alongside ENLIGHTEN™ and called from the RamanID plugin.

It cannot be overemphasized that _this is not a "good" Raman ID algorithm._
It is provided to demonstrate how an ENLIGHTEN™ user can create their own
plugins, even plugins which communicate with external applications at run-time.

More-advanced Raman ID algorithms, with improved "smarts" and accuracy, may be
added to ENLIGHTEN™ over time, whether as plugins or integrated features, 
public or private, but that is beyond the remit and capabilities of this GitHub 
project.

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

# Design

The basic goal of this application was to allow the "spectral library" to be
nothing more than a folder of spectra saved from ENLIGHTEN™ as individual .CSV 
files. 

This makes it extremely simple for users to generate their own "Raman library"
simply by creating a folder and dragging "known-good" spectra into it. Only one
spectrum is required (or supported) per compound.  Compound names are taken from
the "Label" field of the CSV, or filename if that metadata is not provided.
By way of demonstration, sample libraries are included in the libraries/ folder.

The algorithm does nothing more than compare peak locations on the x-axis. Peak
intensity (relative or absolute) is not factored in the comparison.

Given the above, the peak-finding algorithm in use becomes of paramount importance
-- and the peak-finding routine in this application is simple indeed. All the math
can be found in \ref Identify::Library, especially:

- \ref Identify::Library::findPeakWavenumbers
- \ref Identify::Library::checkFit

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

A more accurate algorithm might consider some low-hanging opportunities for 
improvement:

- interpolate library spectra to match the sample measurement's x-axis
  (the current design avoids this by only comparing the x-coordinate of peak 
   centroids)
- generate a Pearson correlation coefficient between the sample measurement 
  and each interpolated library spectrum.

# History

- see [CHANGELOG](README_CHANGELOG.md)
