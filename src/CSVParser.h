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

            // attributes
            std::vector<float> wavenumbers;
            std::vector<float> intensities;
            bool valid = false;

        private:
            // methods
            bool parse(const std::string& pathname);

            inline bool isNum(const std::string& line) const;
            void readHeader(const std::vector<std::string>& tok);
            void readValues(const std::vector<std::string>& tok);

            // attributes
            // std::string state;
            int colWavenumber = 0; // guessing...update if
            int colIntensity  = 1; //   we learn otherwise

    };
}

#endif
