#ifndef IDENTIFY_LIBRARY_H
#define IDENTIFY_LIBRARY_H

#include <map>
#include <vector>
#include <string>

namespace Identify
{
    //! A deliberately simple, naive Raman identification algorithm.
    class Library 
    {
        public:
            //! instantiate a Llibrary with multiple compounds
            Library(const std::string& pathname);

            //! return the index and score of the best-matching compound, if any (neg otherwise)
            int identify(const std::vector<float>& sample, float& score) const;

            //! get the requested compound name
            const std::string& getCompoundName(int i) const;

        private:
            void add(const std::string& name, const std::vector<float>& spectrum);
            float checkFit(const std::vector<int>& samplePeaks, const std::vector<int>& libraryPeaks) const;

            std::vector<int>   findPeakPixels(const std::vector<float>& spectrum, int minRampWidth, int minPeakHeight) const;
            std::vector<float> boxcar        (const std::vector<float>& spectrum, int halfWidth) const;

            std::vector<std::string> compoundNames;
            std::vector<std::vector<int>>  compoundPeaks;
    };
}

#endif
