#include "CSVParser.h"

#include "Util.h"

#include <fstream>
#include <iostream>

using std::vector;
using std::string;

Identify::CSVParser::CSVParser(const string& pathname)
{
    this->pathname = pathname;
    valid = parse(pathname);    
}

//! @todo could clean-up pathname a little (remove .csv etc)
string Identify::CSVParser::getName() const
{
    if (label.size() > 0)
        return label;
    return pathname;
}

bool Identify::CSVParser::isValid() const
{
    return valid;
}

bool Identify::CSVParser::isNum(const string& line) const
{
    if (line.size() == 0)
        return false;
    char c = line[0];
    return ('0' <= c && c <= '9') || c == '-';
}

void Identify::CSVParser::readHeader(const vector<string>& tok)
{
    for (unsigned i = 0; i < tok.size(); i++)
    {
        string s(Util::toLower(tok[i]));
        if (s == "wavenumber")
            colWavenumber = i;
        else if (Util::match(s, "processed|spectrum|spectra|intensity")) // any but "raw"
            colIntensity = i;
    }
}

void Identify::CSVParser::readValues(const vector<string>& tok)
{
    int len = (int)tok.size();
    if ((len < colWavenumber + 1) ||
        (len < colIntensity  + 1))
        return;

    float wavenumber = (float) atof(tok[colWavenumber].c_str());
    float intensity  = (float) atof(tok[colIntensity ].c_str());

    wavenumbers.push_back(wavenumber);
    intensities.push_back(intensity);
}

bool Identify::CSVParser::parse(const string& pathname)
{
    std::ifstream f;
    f.open(pathname, std::ios_base::in);

    enum States { READING_METADATA, READING_DATA, READING_HEADER };
    States state = READING_METADATA;

    string line;
    while (std::getline(f, line))
    {
        Util::trim(line);

        // some files have "CSV blanks" (lines of nothing but commas)
        if (Util::match(line, "^,+$"))
            line = "";

        vector<string> tok = Util::split(line, ",");
        Util::trim(tok);

        // Util::log("read[%-20s]: %s", state.c_str(), line.c_str());

        if (state == READING_METADATA)
        {
            if (isNum(line))
            {
                // We found a digit, so either this file doesn't have metadata, 
                // or we're already past it.  Unfortunately, this probably means
                // we don't know what the field ordering is, so assume defaults.
                state = READING_DATA;
                readValues(tok);
            }
            else if (!line.size())
            {
                // we found a blank, so assume next row is header
                state = READING_HEADER;
            }
            else
            {
                // parse metadata -- the only field we're using right now is "Label"
                if (tok.size() > 1)
                {
                    if (Util::toLower(tok[0]) == "label")
                    {
                        label = tok[1];
                    }
                }
            }
        }
        else if (state == READING_HEADER)
        {
            if (!line.size())
            {
                // skip extra blank
            }
            else if (isNum(line))
            {
                state = READING_DATA;
                readValues(tok);
            }
            else
            {
                readHeader(tok);
                state = READING_DATA;
            }
        }
        else if (state == READING_DATA)
        {
            readValues(tok);
        }
        else
        {
            // Util::log("CSVParser: invalid state");
            return false;
        }
    }
    return true;
}
