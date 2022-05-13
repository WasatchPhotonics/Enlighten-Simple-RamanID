#ifdef _WIN32
#include "save\getopt.h"
#else
#include <getopt.h>
#endif

#include "StreamRequestJSON.h"
#include "Spectrum.h"
#include "Library.h"
#include "Util.h"

#include <string>
#include <list>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

using std::string;
using std::list;

const char* VERSION = "RamanIDAlgo-1.1.0";

//! holds parsed command-line options controlling runtime behavior
struct Options
{
    string libraryPath;     //!< directory containing library spectra
    string logfile;         //!< path to which log should be written
    list<const char*> files;//!< measurements to analyze
    bool help = false;      //!< show help
    bool verbose = false;   //!< include debug output
    bool streaming = false; //!< read streaming spectra from stdin
};

//! display command-line usage
void usage(const char* progname)
{
    printf("%s %s (C) 2022, Wasatch Photonics\n", progname, VERSION);
    printf("\n");
    printf("Usage: %s [--verbose] [--streaming] [--logfile path] --library /path/to/library [sample.csv...]\n", progname);
    printf("       %s --help\n", progname);
    printf("\n");
    printf("NOTE:  This version has been modified from the original in the following key respects:\n");
    printf("\n");
    printf("- Only one library compound is supported\n");
    printf("- All math is single-precision float\n");
    printf("- Memory is static rather than from heap\n");
    printf("- Other optimizations as necessary to reduce memory usage and runtime\n");
    printf("\n");
    printf("Example: %s --library libraries/WP-785 data/WP-785/*.csv\n", progname);
    printf("\n"
           "Options:\n"
           "    --streaming read streaming spectra from stdin\n"
           "    --verbose   include debugging output\n"
           "    --logfile   path to log debug messages\n"
           "\n");               
    exit(1);                    
}                               
                                
/////////////////////////////// /////////////////////////////////////////////////
// Arguments
////////////////////////////////////////////////////////////////////////////////

//! parse command-line arguments into an Options object
Options parseArgs(int argc, char **argv)
{
    Options opts;
    int digit_optind = 0;

    while (1) 
    {
        int option_index = 0;
        static struct option long_options[] = {
           {"help",           no_argument,       0,  0 },
           {"library",        required_argument, 0,  0 },
           {"logfile",        required_argument, 0,  0 },
           {"streaming",      no_argument,       0,  0 },
           {"verbose",        no_argument,       0,  0 },

           // these aren't actually implemented -- required for compatibility with plug-in API
           {"unknown-thresh", required_argument, 0,  0 },

           {0,                0,                 0,  0 }
        };

        // is there no std:: equivalent for this?
        int c = getopt_long(argc, argv, "?", long_options, &option_index);
        if (c == -1)
            break;

        else if (c == 0)
        {
            string key(long_options[option_index].name);
            if (optarg)
            {
                string value(optarg);
                     if (key == "library") opts.libraryPath  = value;
                else if (key == "logfile") opts.logfile      = value;
            }
            else
            {
                     if (key == "help"      ) opts.help      = true;
                else if (key == "streaming" ) opts.streaming = true;
                else if (key == "verbose"   ) opts.verbose   = true;
            }
        }
    }

    while (optind < argc)
        opts.files.push_back(argv[optind++]);

    return opts;
}

////////////////////////////////////////////////////////////////////////////////
// main()
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    // parse args
    Options opts = parseArgs(argc, argv);
    if (opts.help || !opts.libraryPath.size() || (!opts.streaming && !opts.files.size()))
        usage(argv[0]);

    Util::logging_enabled = opts.verbose;
    Util::set_logfile(opts.logfile);

    // initialize library
    Identify::Library library(opts.libraryPath);

    if (opts.streaming)
    {
        ////////////////////////////////////////////////////////////////////////
        // This path is only used from ENLIGHTEN
        ////////////////////////////////////////////////////////////////////////

        // RamanID plugin checks for line containing "ready" (doesn't have to be in JSON)
        printf("{ \"Status\": \"ready\" }\n"); 
        while (true)
        {
            if (std::cin.eof())
                break;

            try
            {
                Identify::StreamRequestJSON request(std::cin);
                if (!request.valid || request.isQuit)
                {
                    Util::log("main: bad request (valid %s, isQuit %s)", 
                        request.valid  ? "true" : "false", 
                        request.isQuit ? "true" : "false");
                    break;
                }

                // original
                float score = 0;
                string name = library.identify(request.spectrum, score);
                if (name.size() >= 0 && score >= request.min_confidence)
                    printf("{ \"MatchResult\": [ { \"Name\": \"%s\", \"Score\": %.2f } ] }\n", name.c_str(), score);
                else
                    printf("{ \"MatchResult\": [ ] }\n");
            }
            catch (std::exception &e)
            {
                Util::log("ERROR: exception parsing streamed input: %s", e.what());
                break;
            }
        }
        printf("{ \"Status\": \"done\" }\n"); 
    }
    else
    {
        // process each spectrum on the cmd-line
        Util::log("------------------------------------------");
        Util::log("Processing input files");
        Util::log("------------------------------------------");

        for (auto& pathname : opts.files)
        {
            struct stat s;
            if (stat(pathname, &s) == 0)
            {
                if (s.st_mode & S_IFDIR)
                {
                    Util::log("Skipping directory");
                    continue;
                }
            }

            // load the measurement to process
            Identify::Spectrum measurement(pathname);

            float score = 0;
            string name = library.identify(measurement, score);
            if (name.size() > 0)
                printf("sample %s: matched library %s with score %.2f\n", measurement.name.c_str(), name.c_str(), score);
            else
                printf("sample %s: NO MATCH\n", measurement.name.c_str());
            
            Util::log("");
        }
    }
}
