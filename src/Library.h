#ifndef IDENTIFY_LIBRARY_H
#define IDENTIFY_LIBRARY_H

#include <map>
#include <vector>
#include <string>

#include "Spectrum.h"
#include "LibrarySpectrum.h"

namespace Identify
{
    //! A deliberately simple, naive Raman identification algorithm.
    class Library 
    {
        public:
            //! instantiate a Llibrary with multiple compounds
            Library(const std::string& pathname);

            //! return the name and score of the best-matching compound, if any (neg otherwise)
            std::string identify(const Identify::Spectrum& sample, float& score) const;

        private:
            void add(const Identify::Spectrum& spectrum);
            float checkFit(const std::vector<float>& samplePeaks, const std::vector<float>& libraryPeaks) const;

            std::vector<float> findPeakWavenumbers(const Identify::Spectrum& spectrum, int boxcar, int minRampWidth, int minPeakHeight) const;
            std::vector<float> boxcar(const std::vector<float>& spectrum, int halfWidth) const;

            std::map<std::string, std::vector<float>> libraryPeakWavenumbers;
    };
}

#endif
