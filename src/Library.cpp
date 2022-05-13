#include <list>

#include "Library.h"
#include "Spectrum.h"
#include "Util.h"

#define BOXCAR_LIBRARY           10 // used to smooth library spectrum for peakfinding
#define MIN_RAMP_PIXELS_LIBRARY  15
#define MIN_PEAK_HEIGHT_LIBRARY 500 // above start of left incline

#define BOXCAR_SAMPLE             5 // used to smooth sample spectrum 
#define MIN_RAMP_PIXELS_SAMPLE    5
#define MIN_PEAK_HEIGHT_SAMPLE  100 // above start of left incline

#define MAX_WAVENUMBER_OFFSET    10 // allow sample peaks to shift this much from library

using std::map;
using std::list;
using std::string;
using std::vector;
using std::make_pair;

const string unknownCompound("UNKNOWN");

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                 Lifecycle                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

Identify::Library::Library(const string& dir)
{
    vector<string> filenames = Util::readDir(dir);
    for (auto& filename : filenames)
    {
        if (!Util::endsWith(filename, ".csv"))
            continue;

        Identify::Spectrum spectrum(dir + "/" + filename);
        add(spectrum);
    }
}

void Identify::Library::add(const Spectrum& spectrum)
{
    auto peakWavenumbers = findPeakWavenumbers(spectrum, BOXCAR_LIBRARY, MIN_RAMP_PIXELS_LIBRARY, MIN_PEAK_HEIGHT_LIBRARY);
    if (peakWavenumbers.size() == 0)
        return;

    libraryPeakWavenumbers.insert(std::make_pair(spectrum.name, peakWavenumbers));
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                  Methods                                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/**
    Compare the passed 'sample' spectrum against the library spectra.  If any 
    match, return the index of the best-matching library compound and populate
    'score' with an approximate confidence rating.

    @param score (output) confidence score (higher is better, range 0 .. 1)
    @returns negative on failure (no match), else best matching compound index
             (range 0 .. (n-1))
*/
string Identify::Library::identify(const Spectrum& sample, float& score) const
{
    score = -1;

    auto samplePeakWavenumbers = findPeakWavenumbers(sample, BOXCAR_SAMPLE, MIN_RAMP_PIXELS_SAMPLE, MIN_PEAK_HEIGHT_SAMPLE);

    // no match possible
    if (samplePeakWavenumbers.size() < 1)
    {
        Util::log("identify: no sample peaks found");
        return "";
    }

    Util::log("identify: sample peak wavenumbers: %s", Util::join(samplePeakWavenumbers, ", ").c_str());

    Util::log("identify: Computing fitness of each library compound");
    float bestScore = 0;
    string bestCompound;
    for (auto it = libraryPeakWavenumbers.begin(); it != libraryPeakWavenumbers.end(); ++it)
    {
        const string& name = it->first;

        Util::log("identify: computing fitness of %s...", name.c_str());
        float thisScore = checkFit(samplePeakWavenumbers, it->second);

        Util::log("identify: %s thisScore = %.2f", name.c_str(), thisScore);
        if (thisScore == 0)
            continue;

        if (bestScore < thisScore)
        {
            bestScore = thisScore;
            bestCompound = name;
            Util::log("identify: best score now %.2f (best compound %s)", bestScore, bestCompound.c_str());
        }
    }

    score = bestScore;
    Util::log("identify: returning compound %s (score %.2f)", bestCompound.c_str(), score);
    return bestCompound;    
}

inline float absDiff(float a, float b) { return a < b ? b - a : a - b; }

//! @returns goodness of fit between two lists of peaks (normalized to range (0, 100), 100 being best)
float Identify::Library::checkFit(const vector<float>& samplePeaks, const vector<float>& libraryPeaks) const
{
    if (libraryPeaks.size() == 0)
    {
        Util::log("checkFit: library compound has no peaks");
        return 0;
    }

    if (samplePeaks.size() < libraryPeaks.size())
    {
        Util::log("checkFit: sample has too few peaks (%d < %d)", samplePeaks.size(), libraryPeaks.size());
        return 0;
    }

    // for each library peak, find closest matching sample peak
    float possibleScore = 100.0f / libraryPeaks.size();
    float totalScore = 0;
    for (int i = 0; i < (int)libraryPeaks.size(); i++)
    {
        float lp = libraryPeaks[i];

        float minDist = absDiff(lp, samplePeaks[0]);
        int bestIndex = 0;
        for (int j = 1; j < (int)samplePeaks.size(); j++)
        {
            float dist = absDiff(lp, samplePeaks[j]);
            if (minDist > dist)
            {
                minDist = dist;
                bestIndex = j;
            }
            // else break? (getting further away...)
        }

        float sp = samplePeaks[bestIndex];
        float dist = lp - sp;
        Util::log("checkFit: best fit for library peak #%2d (wavenumber %.2f) is sample peak #%2d (wavenumber %.2f) for dist %.2f", i, lp, bestIndex, sp, dist);

        if (minDist > MAX_WAVENUMBER_OFFSET)
        {
            Util::log("checkFit: unable to associate library peak %d with any sample peak (minDist %.2f > thresh %d)", minDist, MAX_WAVENUMBER_OFFSET);
            return 0;
        }

        float peakScore = possibleScore * (1.f - 1.f * minDist / MAX_WAVENUMBER_OFFSET);
        totalScore += peakScore;

        Util::log("checkFit: peakScore %8.2f (total %8.2f)", peakScore, totalScore);
    }

    Util::log("checkFit: returning totalScore %.2f", totalScore);
    return totalScore;
}

/**
 We're simplistically defining a peak as one which is the highest for at least
 MIN_RAMP_PIXELS in either direction, as well as MIN_PEAK_HEIGHT counts above the
 left-hand shoulder.
 */
vector<float> Identify::Library::findPeakWavenumbers(const Spectrum& spectrum, int boxcarHalfWidth, int minRampPixels, int minPeakHeight) const
{
    vector<float> peakWavenumbers;
    vector<float> intensities = boxcar(spectrum.intensities, boxcarHalfWidth);

    int rampLeft = 0;
    float rampBase = intensities[0];
    for (int i = 1; i < (int)spectrum.pixels - minRampPixels; i++)
    {
        // is ramp increasing?
        if (intensities[i] > intensities[i-1])
        {
            rampLeft++;
            if (rampLeft >= minRampPixels)
            {
                // we satisfied the left-ramp, now see if the right-ramp 
                // indicates we're at a local maxima
                bool maxima = true;
                for (int j = i; j < i + minRampPixels && maxima; j++)
                    if (intensities[j] <= intensities[j+1])
                        maxima = false;
                if (maxima)
                {
                    // apparently this is a local maxima

                    // is it HIGH ENOUGH?
                    if (intensities[i] >= rampBase + minPeakHeight)
                        peakWavenumbers.push_back(spectrum.wavenumbers[i]);
                }
            }
        }
        else
        {
            rampLeft = 0;
            rampBase = intensities[0];
        }
    }
    return peakWavenumbers;
}

vector<float> Identify::Library::boxcar(const vector<float>& a, int halfWidth) const
{
	vector<float> smoothed(a.size());
	for (int i = 0; i < (int)a.size(); i++)
    {
        if (i < halfWidth || i + halfWidth >= (int)a.size())
            smoothed[i] = a[i];
        else
        {
            float sum = 0;
            for (int j = i - halfWidth; j <= i + halfWidth; j++)
                sum += a[j];
            smoothed[i] = sum / (halfWidth * 2 + 1);
        }
    }
    return smoothed;
}
