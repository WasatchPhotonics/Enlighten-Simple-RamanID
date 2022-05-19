#ifndef IDENTIFY_LIBRARY_SPECTRUM
#define IDENTIFY_LIBRARY_SPECTRUM

#include <string>
#include <vector>

namespace Identify
{
    class LibrarySpectrum
    {
        // methods
        public:
            LibrarySpectrum(const std::string& name, const std::vector<float> peakWavenumbers);

        // attributes
        public:
            std::string name;
            std::vector<float> peakWavenumbers;
    };
}

#endif