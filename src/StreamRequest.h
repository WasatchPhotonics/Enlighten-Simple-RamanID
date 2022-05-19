#ifndef IDENTIFY_STREAM_REQUEST_H
#define IDENTIFY_STREAM_REQUEST_H

#include "Spectrum.h"

#include <string>
#include <vector>
#include <istream>

namespace Identify
{
    class StreamRequest
    {
        public:
            StreamRequest(std::istream& infile);
            virtual ~StreamRequest();

            Spectrum spectrum;
            float min_confidence = 0;
            int max_results = 20;
            bool isQuit = false;
            bool valid = false;

        protected:
            // abstract method
            virtual bool load(std::istream& infile) = 0;
    };
}

#endif
