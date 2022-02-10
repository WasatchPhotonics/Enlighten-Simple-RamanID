#ifndef IDENTIFY_UTIL_H
#define IDENTIFY_UTIL_H

#ifdef __cplusplus
extern "C" 
{
#endif
    void log_spectrum(const char *label, int pixels, const float* spectrum);
#ifdef __cplusplus
}


#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>
#include <string>
#include <sstream>

/*! @brief Simple utility functions.

    @see trim functions from https://stackoverflow.com/a/217605

    Deliberately not in a namespace.
*/
class Util
{
    public:

        ////////////////////////////////////////////////////////////////////////
        // Logging
        ////////////////////////////////////////////////////////////////////////

        static bool logging_enabled;
        static std::string timestamp();
        static void log(const char* format, ...);
        static void pause(const std::string& msg);
        static void log_spectrum(const std::string& label, const std::vector<float>& spectrum);

        ////////////////////////////////////////////////////////////////////////
        // Files
        ////////////////////////////////////////////////////////////////////////

        static std::string basename(const std::string& pathname);
        static std::vector<std::string> readDir(const std::string& path);

        ////////////////////////////////////////////////////////////////////////
        // Strings
        ////////////////////////////////////////////////////////////////////////

        // constructing
        static std::string sprintf(const char* format, ...);

        // case
        static std::string toLower(const std::string& s);

        // splitting
        static std::vector<std::string> split(const std::string& s, const std::string& delim);

        // joining
        static std::string join(const std::vector<float>& v, const std::string& delim);
        template<typename T> static std::string join(const T& v, const std::string& delim)
        {
            std::stringstream ss;
            bool first = true;
            for (auto s : v)
            {
                if (!first)
                    ss << delim;
                ss << s;
                first = false;
            }
            return ss.str();
        }

        // searching
        static bool match(const std::string& s, const std::string& pattern);
        static bool endsWith(const std::string& s, const std::string& suffix)
        {
            std::size_t pos = s.rfind(suffix);
            if (pos == std::string::npos)
                return false;
            return pos == s.size() - suffix.size();
        }

        static bool startsWith(const std::string& s, const std::string& prefix)
        {
            return 0 == s.rfind(prefix, 0); 
        }

        // trimming 

        //! trim from start (in place)
        static inline void ltrim(std::string &s) 
        {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) 
            {
                return !std::isspace(ch);
            }));
        }

        //! trim from end (in place)
        static inline void rtrim(std::string &s) 
        {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) 
            {
                return !std::isspace(ch);
            }).base(), s.end());
        }

        //! trim from both ends (in place)
        static inline void trim(std::string &s) 
        {
            ltrim(s);
            rtrim(s);
        }

        static inline void trim(std::vector<std::string> &v) 
        {
            for (auto s : v)
                trim(s);
        }

        //! trim from start (copying)
        static inline std::string ltrim_copy(std::string s) 
        {
            ltrim(s);
            return s;
        }

        //! trim from end (copying)
        static inline std::string rtrim_copy(std::string s) 
        {
            rtrim(s);
            return s;
        }

        //! trim from both ends (copying)
        static inline std::string trim_copy(std::string s) 
        {
            trim(s);
            return s;
        }
};

#endif // __cplusplus

#endif // IDENTIFY_UTIL_H
