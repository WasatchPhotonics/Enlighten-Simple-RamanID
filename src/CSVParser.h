#ifndef IDENTIFY_CSV_PARSER_H
#define IDENTIFY_CSV_PARSER_H

#include <string>
#include <vector>

namespace Identify
{
    class CSVParser
    {
        public:
            // methods
            CSVParser(const std::string& pathname);
            std::string getName() const;
            bool isValid() const;

            // attributes
            std::vector<float> wavenumbers;
            std::vector<float> intensities;

        private:
            // methods
            bool parse(const std::string& pathname);

            inline bool isNum(const std::string& line) const;
            void readHeader(const std::vector<std::string>& tok);
            void readValues(const std::vector<std::string>& tok);

            // attributes
            bool valid = false;
            std::string pathname;
            std::string label;
            int colWavenumber = 0; // initial defaults (dynamically
            int colIntensity  = 1; // overwritten if header row found)
    };
}

#endif
