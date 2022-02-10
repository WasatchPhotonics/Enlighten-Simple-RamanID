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
            int pixels = 1024;

            std::string pathname;
            std::string name;

            std::vector<float> wavenumbers;
            std::vector<float> intensities;

            //! instantiate a spectrum from a CSV file containing (wavenumber, intensity) pairs
            Spectrum(const std::string& pathname);

            // Spectrum(const std::vector<float>& wavenumbers_in, 
            //          const std::vector<float>& intensities_in);

            //! interpolate a spectrum using the defined range of wavenumbers
            // Spectrum interpolate(float start, float end, float incr) const;

            //! interpolate a spectrum using the given set of wavenumbers
            // Spectrum interpolate(const std::vector<float>& wavenumbers) const;
    };
}

#endif
