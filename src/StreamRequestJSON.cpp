#include "StreamRequestJSON.h"

#include "Util.h"

#include "simdjson.h"

using namespace simdjson;

using std::string;
using std::istream;

ondemand::parser Identify::StreamRequestJSON::parser;

Identify::StreamRequestJSON::StreamRequestJSON(istream& is)
    : Identify::StreamRequest(is)
{
    Util::log("instantiating StreamRequestJSON");
    valid = load(is);
    Util::log("instantiated StreamRequestJSON (valid %s)", valid ? "yes" : "no");
}

Identify::StreamRequestJSON::~StreamRequestJSON()
{
    Util::log("destroying StreamRequestJSON");
}

bool Identify::StreamRequestJSON::load(istream& is)
{
    if (is.eof())
    {
        Util::log("StreamRequestJSON: EOF");
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////
    // read ONE LINE from input stream (i.e. an NDJSON document)
    ////////////////////////////////////////////////////////////////////////////

    string json;
    std::getline(is, json);
    if (json.size() == 0)
    {
        Util::log("StreamRequestJSON: empty");
        return false;
    }
    json.reserve(json.size() + SIMDJSON_PADDING);

    ////////////////////////////////////////////////////////////////////////////
    // parse JSON (create document iterator)
    ////////////////////////////////////////////////////////////////////////////

    ondemand::document doc = parser.iterate(json);

    // note that simdjson runs faster if keys are searched in order of 
    // appearance, so recommend both client and server sort keys alphabetically

    // max_results (optional)
    auto max_results_obj = doc["max_results"];
    if (NO_SUCH_FIELD == max_results_obj.error())
        Util::log("StreamRequestJSON: missing max_results");
    else
        max_results = double(max_results_obj);

    // min_confidence (optional)
    auto min_confidence_obj = doc["min_confidence"];
    if (NO_SUCH_FIELD == min_confidence_obj.error())
        Util::log("StreamRequestJSON: missing min_confidence");
    else
        min_confidence = double(min_confidence_obj);

    // spectrum (required)
    auto spectrum_obj = doc["spectrum"];
    if (NO_SUCH_FIELD == spectrum_obj.error())
    {
        Util::log("StreamRequestJSON: missing spectrum");
        return false;
    }
    for (auto value: spectrum_obj)
        spectrum.intensities.push_back(double(value));

    // wavenumbers (required)
    auto wavenumbers_obj = doc["wavenumbers"];
    if (NO_SUCH_FIELD == wavenumbers_obj.error())
    {
        Util::log("StreamRequestJSON: missing wavenumbers");
        return false;
    }
    for (auto value: wavenumbers_obj)
        spectrum.wavenumbers.push_back(double(value));

    spectrum.pixels = spectrum.wavenumbers.size();
    Util::log("read JSON request with %d wavenumbers (%.2f, %.2f), %d intensities, min_confidence %.2f and max_results %d",
        spectrum.pixels,
        spectrum.pixels > 0 ? spectrum.wavenumbers[        0        ] : -1,
        spectrum.pixels > 0 ? spectrum.wavenumbers[spectrum.pixels-1] : -1,
        spectrum.intensities.size(), 
        min_confidence, 
        max_results);

    bool ok = spectrum.isValid();
    Util::log("spectrum valid = %s", ok ? "yes" : "no");
    return ok;
}
