#ifndef IDENTIFY_STREAM_REQUEST_JSON_H
#define IDENTIFY_STREAM_REQUEST_JSON_H

#include "StreamRequest.h"
#include "simdjson.h"

namespace Identify
{
    class StreamRequestJSON : public StreamRequest
    {
        public:
            StreamRequestJSON(std::istream& infile);
            virtual ~StreamRequestJSON();

        private:
            virtual bool load(std::istream& infile);

            //! for efficiency, re-use parser over multiple input requests
            //! @see https://github.com/simdjson/simdjson/blob/master/doc/basics.md#parser-document-and-json-scope
            static simdjson::ondemand::parser parser;
    };
}

#endif
