#ifndef IDENTIFY_SPECTRUM_H
#define IDENTIFY_SPECTRUM_H

#include <string>
#include <vector>
#include <istream>

namespace Identify
{
    class Spectrum
    {
        public:
            int pixels;

            std::string pathname;
            std::string name;

            std::vector<float> wavenumbers;
            std::vector<float> intensities;

        public:
            //! used by StreamRequestJSON
            Spectrum(); 

            //! instantiate a spectrum from a CSV file containing (wavenumber, intensity) pairs
            Spectrum(const std::string& pathname);

            bool isValid();
    };
}

#endif
