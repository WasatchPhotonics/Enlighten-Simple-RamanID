#ifndef IDENTIFY_INTERIM_H
#define IDENTIFY_INTERIM_H

#include <map>
#include <vector>
#include <string>

namespace Identify
{
    /**
    * @brief    An interim, simpler matching 
    *           for early system testing on limited-resource STM32F405 micro-
    *           processors.
    */
    class Interim
    {
        public:

            //! instantiate an "authenticate-only" Interim library with one compound
            Interim(const std::vector<float>& spectrum);

            //! instantiate an ID-capable Interim library with multiple compounds
            Interim(const std::string& pathname);

            //! return the index and score of the best-matching compound, if any (neg otherwise)
            int identify(const std::vector<float>& sample, float& score) const;

            //! get the requested compound name
            const std::string& getCompoundName(int i) const;

        private:
            void add(const std::string& name, const std::vector<float>& spectrum);
            float checkFit(const std::vector<int>& samplePeaks, const std::vector<int>& libraryPeaks) const;

            std::vector<int>   findPeakPixels(const std::vector<float>& spectrum, int minRampWidth, int minPeakHeight) const;
            std::vector<float> boxcar        (const std::vector<float>& spectrum, int halfWidth) const;

            // std::map<const std::string, const std::vector<int>> compoundPeaksByName;
            std::vector<std::string> compoundNames;
            std::vector<std::vector<int>>  compoundPeaks;
    };
}

#endif
