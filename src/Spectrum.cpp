#include "Spectrum.h"

#include "CSVParser.h"
#include "Util.h"

using std::string;

Identify::Spectrum::Spectrum()
{
    name = "no-name";
    pixels = 0;
}

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
        Util::log("loaded %s (%d pixels)", name.c_str(), pixels);
    }
}

bool Identify::Spectrum::isValid()
{
    if (pixels != wavenumbers.size())
        return false;
    if (wavenumbers.size() != intensities.size())
        return false;
    return true;
}
