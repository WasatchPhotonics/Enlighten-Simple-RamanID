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
    if (parser.isValid())
    {
        name = parser.getName();
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

float Identify::Spectrum::getIntensityAtWavenumber(float wavenumber) const
{
    // deal with extrapolation off the ends
    if (wavenumber <= wavenumbers[0])
        return intensities[0];
    if (wavenumber >= wavenumbers[pixels - 1])
        return intensities[pixels - 1];

    // find the bracketing pixels
    int i = 0;
    while (wavenumbers[i + 1] <= wavenumber)
        ++i;

    // linear interpolation
    float x0 = wavenumbers[i];
    float x1 = wavenumbers[i + 1];
    float y0 = intensities[i];
    float y1 = intensities[i + 1];

    float y = y0 + (y1 - y0) * (wavenumber - x0) / (x1 - x0);

    Util::log("interpolated wavenumber %8.2f to intensity %8.2f (bracketing wavenumbers %8.2f, %8.2f, intensities %8.2f, %8.2f",
        wavenumber, y, x0, x1, y0, y1);

    return y;
}
