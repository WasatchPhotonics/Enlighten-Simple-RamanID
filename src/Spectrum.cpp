#include "Spectrum.h"

#include "CSVParser.h"
//#include "Interpolator.h"
#include "Util.h"

#include <iostream>
#include <exception>

using std::vector;
using std::string;
using std::istream;
using std::runtime_error;

/*
Identify::Spectrum::Spectrum(
        const vector<float>& wavenumbers_in, 
        const vector<float>& intensities_in) 
    : wavenumbers(wavenumbers_in),
      intensities(intensities_in)
{
    pixels = intensities.size();
}
*/

//! load a spectrum from a CSV file (requires comma delimitors)
Identify::Spectrum::Spectrum(const string& pathname_in) :
    pathname(pathname_in)
{
    name = Util::basename(pathname);

    Identify::CSVParser parser(pathname);
    if (parser.valid)
    {
        wavenumbers = parser.wavenumbers;
        intensities = parser.intensities;
        pixels = wavenumbers.size();
        // Util::log("loaded %s (%d pixels)", name.c_str(), pixels);
    }
}
