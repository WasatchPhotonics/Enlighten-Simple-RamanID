#ifdef _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

#include "Util.h"

#include <time.h>
#include <stdarg.h>

#include <sstream>
#include <regex>

using std::regex;
using std::smatch;
using std::vector;
using std::string;
using std::stringstream;
using std::regex_search;

bool Util::logging_enabled = false;

string Util::join(const vector<float>& v, const string& delim)
{
    bool first = true;
    stringstream ss;
    for (auto d : v)
    {
        if (!first)
            ss << delim;
        ss << Util::sprintf("%.2lf", d);
        first = false;
    }
    return ss.str();
}

vector<string> Util::split(const string& s, const string& delim)
{
    vector<string> tok;
    size_t prev = 0;
	size_t pos = 0;
    do
    {
        pos = s.find(delim, prev);
        if (pos == string::npos) 
			pos = s.length();
		tok.push_back(s.substr(prev, pos - prev));
        prev = pos + delim.length();
    }
    while (pos < s.length() && prev < s.length());

    return tok;
}

string Util::toLower(const string& s)
{
    string lc(s);
    for (unsigned i = 0; i < lc.size(); i++)
        lc[i] = tolower(lc[i]);
    return lc;
}

string Util::sprintf(const char* fmt, ...)
{
    char buf[256];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return string(buf);
}

string Util::timestamp()
{
    char buf[32] = { 0 };
    time_t now = time(NULL);
    string(ts) = ctime(&now);
    ts[ts.length() - 1] = 0;
    snprintf(buf, sizeof(buf), "%s ", ts.c_str());
    return string(buf);
}

//! print a single timestamped log line to console with linefeed
void Util::log(const char* fmt, ...)
{
    if (!logging_enabled)
        return;

    time_t now = time(NULL);
    string(ts) = ctime(&now);
    ts[ts.length() - 1] = 0;
    printf("DEBUG: %s ", ts.c_str());

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");

    fflush(stdout);
}

void Util::log_spectrum(const string& label, const vector<float>& spectrum)
{
    if (!Util::logging_enabled)
        return;

    Util::log("DEBUG: %s, %s", label.c_str(), Util::join(spectrum, ", ").c_str());
}

/**
    @brief given /path/to/foo.bar.csv, returns "foo.bar"
*/
string Util::basename(const string& pathname)
{
    vector<string> tok = split(pathname, "/");
    string filename = tok[tok.size() - 1];

    vector<string> tok2 = split(filename, ".");
    if (tok2.size() > 1)
        tok2.pop_back();

    return join(tok2, ".");
}

//! auto-skip filenames starting with a period
vector<string> Util::readDir(const string& path)
{
    vector<string> files;
    DIR* dirp = opendir(path.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)))
        if (dp->d_name[0] != '.')
            files.push_back(dp->d_name);
    closedir(dirp);
    sort(files.begin(), files.end());
    return files;
}

void Util::pause(const string& msg)
{
    return;
    log("PAUSE: %s", msg.c_str());
    char c = getc(stdin);
}

bool Util::match(const string& s, const string& pattern)
{
	try 
    {
	    regex re(pattern);
	    smatch match;
	    if (regex_search(s, match, re))
		    return true;
	} 
    catch (std::regex_error& e) 
    {
	    log("ERROR: bad regex: %s (%s)", pattern.c_str(), e.what());
	}
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                 C API                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void log_spectrum(const char *label, int pixels, const float* spectrum)
{
    if (!Util::logging_enabled)
        return;

    vector<float> v;
    for (int i = 0; i < pixels; i++)
        v.push_back(spectrum[i]);
    Util::log("DEBUG: %s, %s", label, Util::join(v, ", ").c_str());
}
