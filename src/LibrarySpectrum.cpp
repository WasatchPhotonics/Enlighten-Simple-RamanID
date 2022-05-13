#include "LibrarySpectrum.h"

using std::string;
using std::vector;

Identify::LibrarySpectrum::LibrarySpectrum(const string& name, vector<float> peakWavenumbers)
    : name(name), peakWavenumbers(peakWavenumbers)
{
}
