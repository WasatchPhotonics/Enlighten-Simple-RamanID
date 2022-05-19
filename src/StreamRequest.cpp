#include "StreamRequest.h"

#include "Util.h"

using std::istream;

Identify::StreamRequest::StreamRequest(istream& is)
{
    Util::log("creating StreamRequest");
}

Identify::StreamRequest::~StreamRequest()
{
    Util::log("destroying StreamRequest");
}
