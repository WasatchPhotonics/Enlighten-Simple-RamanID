#ifdef _WIN32
#include "save\getopt.h"
#else
#include <getopt.h>
#endif

#include "Spectrum.h"
//#include "Library.h"
#include "Interim.h"
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

const char* VERSION = "Handheld 1.0";

//! holds parsed command-line options controlling runtime behavior
struct Options
{
    string libraryPath;     //!< directory containing library spectra
    list<const char*> files;//!< measurements to analyze
    bool alt = false;       //!< output alternates
    bool brief = false;     //!< output on one line
    bool help = false;      //!< show help
    bool unknown = false;   //!< output unknown residue
    bool verbose = false;   //!< include debug output
    bool streaming = false; //!< read streaming spectra from stdin
    bool interim = false;   //!< use Interim algorithm
    bool auth = false;      //!< authenticate only
    float thresh = 0.95;    //!< unknown_threshold
};

//! display command-line usage
void usage(const char* progname)
{
    printf("%s %s (C) 2021, Wasatch Photonics\n", progname, VERSION);
    printf("\n");
    printf("Usage: %s [--brief] [--verbose] [--alt] [--unknown] [--thresh frac] [--streaming] --library /path/to/library [sample.csv...]\n", progname);
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
           "    --alt       output match 'alternates'\n"
           "    --brief     output results on one line per sample\n"
           "    --streaming read streaming spectra from stdin\n"
           "    --thresh    unknown threshold (default 0.95)\n"
           "    --unknown   output unknown spectral residue\n"
           "    --verbose   include debugging output\n"
           "    --auth      authenticate only\n"
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
           {"alt",       no_argument,       0,  0 },
           {"brief",     no_argument,       0,  0 },
           {"help",      no_argument,       0,  0 },
           {"library",   required_argument, 0,  0 },
           {"streaming", no_argument,       0,  0 },
           {"thresh",    required_argument, 0,  0 },
           {"unknown",   no_argument,       0,  0 },
           {"verbose",   no_argument,       0,  0 },
           {"auth",      no_argument,       0,  0 },
           {0,           0,                 0,  0 }
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
                if (key == "library")
                    opts.libraryPath = value;
                else if (key == "thresh")
                    opts.thresh = atof(value.c_str());
            }
            else
            {
                     if (key == "alt"       ) opts.alt       = true;
                else if (key == "brief"     ) opts.brief     = true;
                else if (key == "help"      ) opts.help      = true;
                else if (key == "streaming" ) opts.streaming = true;
                else if (key == "unknown"   ) opts.unknown   = true;
                else if (key == "verbose"   ) opts.verbose   = true;
                else if (key == "auth"      ) opts.auth      = true;
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

    // initialize identification library
    /*
    Identify::Library identifyLibrary(opts.libraryPath);
    if (identifyLibrary.size() < 1)
    {
        printf("ERROR: unable to instantiate library\n");
        return -1;
    }
    */

    // initialize Interim solution
    Identify::Interim interimLibrary(opts.libraryPath);

    // process each spectrum
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

		int index = interimLibrary.identify(measurement.intensities);
		if (index >= 0)
		{
			const string& name = interimLibrary.getCompoundName(index);
			printf("Interim: sample %s: matched library %s\n", measurement.name.c_str(), name.c_str());
		}
		else
			printf("Interim: sample %s: NO MATCH\n", measurement.name.c_str());
        
        Util::log("");
    }
}
