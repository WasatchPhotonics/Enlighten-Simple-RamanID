#include <list>

#include "Interim.h"
#include "Spectrum.h"
#include "Util.h"

#define BOXCAR_LIBRARY           10 // used to smooth library spectrum for peakfinding
#define MIN_RAMP_PIXELS_LIBRARY  15
#define MIN_PEAK_HEIGHT_LIBRARY 500 // above start of left incline

#define BOXCAR_SAMPLE             5 // used to smooth sample spectrum 
#define MIN_RAMP_PIXELS_SAMPLE    5
#define MIN_PEAK_HEIGHT_SAMPLE  100 // above start of left incline

#define MAX_PIXEL_OFFSET         20 // allow sample peaks to shift this much from library

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

Identify::Interim::Interim(const vector<float>& spectrum)
{
    add("default", spectrum);
}

Identify::Interim::Interim(const string& dir)
{
    vector<string> filenames = Util::readDir(dir);
    for (auto& filename : filenames)
    {
        if (!Util::endsWith(filename, ".csv"))
            continue;

        Identify::Spectrum spectrum(dir + "/" + filename);
        add(spectrum.name, spectrum.intensities);
    }
}

void Identify::Interim::add(const string& name, const vector<float>& spectrum)
{
    auto smoothed = boxcar(spectrum, BOXCAR_LIBRARY);
    auto peakPixels = findPeakPixels(smoothed, MIN_RAMP_PIXELS_LIBRARY, MIN_PEAK_HEIGHT_LIBRARY);

    if (peakPixels.size() == 0)
    {
        // Util::log("Interim::add(%s): no peaks found", name.c_str());
        return;
    }

    // compoundPeaksByName.insert(make_pair(name, peakPixels));
    compoundNames.push_back(name);
    compoundPeaks.push_back(peakPixels);

    // Util::log("Interim::add(%s): %s", name.c_str(), Util::join(peakPixels, ", ").c_str());
}

const string& Identify::Interim::getCompoundName(int i) const
{
    if (i >= 0 && i < (int)compoundNames.size())
        return compoundNames.at(i);
    return unknownCompound;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                  Methods                                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/*! 
    Compare the passed 'sample' spectrum against the library spectra.  If any 
    match, return the index of the best-matching library compound and populate
    'score' with an approximate confidence rating.

    @param score (output) confidence score (higher is better, range 0 .. 1)
    @returns negative on failure (no match), else best matching compound index
             (range 0 .. (n-1))
*/
int Identify::Interim::identify(const vector<float>& sample, float& score) const
{
    score = 0;

    auto smoothed = boxcar(sample, BOXCAR_SAMPLE);
    auto samplePeaks = findPeakPixels(smoothed, MIN_RAMP_PIXELS_SAMPLE, MIN_PEAK_HEIGHT_SAMPLE);

    // no match possible
    if (samplePeaks.size() < 1)
    {
        // Util::log("identify: no sample peaks found");
        return -1;
    }

    Util::log("identify: sample peaks: %s", Util::join(samplePeaks, ", ").c_str());

    Util::log("identify: Computing fitness of each library compound");
    float bestScore = 0;
    int bestCompound = -1;
    for (int i = 0; i < (int)compoundNames.size(); i++)
    {
        const string& name = compoundNames.at(i);
        const vector<int>& libraryPeaks = compoundPeaks.at(i);

        Util::log("identify: computing fitness of %s...", name.c_str());
        float thisScore = checkFit(samplePeaks, libraryPeaks);

        Util::log("identify: %s thisScore = %.2f", name.c_str(), thisScore);
        if (thisScore == 0)
            continue;

        if (bestScore < thisScore)
        {
            Util::log("identify: best score now %.2f", bestScore);
            bestScore = thisScore;
            bestCompound = i;
        }
    }

    score = bestScore;
    Util::log("identify: returning compound %d (score %.2f)", bestCompound, score);
    return bestCompound;    
}

inline int absDiff(int a, int b) { return a < b ? b - a : a - b; }

//! @returns goodness of fit between two lists of peaks (normalized to range (0, 1), 1 being best)
float Identify::Interim::checkFit(const vector<int>& samplePeaks, const vector<int>& libraryPeaks) const
{
    if (libraryPeaks.size() == 0)
    {
        // Util::log("checkFit: library compound has no peaks");
        return 0;
    }

    if (samplePeaks.size() < libraryPeaks.size())
    {
        // Util::log("checkFit: sample has too few peaks (%d < %d)", samplePeaks.size(), libraryPeaks.size());
        return 0;
    }

    // for each library peak, find closest matching sample peak
    int sumDist = 0;
    int sumDistPos = 0;

    float possibleScore = 1.0f / libraryPeaks.size();
    float totalScore = 0;
    for (int i = 0; i < (int)libraryPeaks.size(); i++)
    {
        int lp = libraryPeaks[i];

        int minDist = absDiff(lp, samplePeaks[0]);
        int bestIndex = 0;
        for (int j = 1; j < (int)samplePeaks.size(); j++)
        {
            int dist = absDiff(lp, samplePeaks[j]);
            if (minDist > dist)
            {
                minDist = dist;
                bestIndex = j;
            }
            // else break? (getting further away...)
        }

        int sp = samplePeaks[bestIndex];
        int dist = lp - sp;
        // Util::log("checkFit: best fit for library peak #%2d (pixel %4d) is sample peak #%2d (pixel %4d) for dist %4d", i, lp, bestIndex, sp, dist);

        if (minDist > MAX_PIXEL_OFFSET)
        {
            // Util::log("checkFit: unable to associate library peak %d with any sample peak (minDist %d > thresh %d)", minDist, MAX_PIXEL_OFFSET);
            return 0;
        }

        sumDist += dist;
        sumDistPos += minDist;

        float peakScore = possibleScore * (1.f - 1.f * minDist / MAX_PIXEL_OFFSET);
        totalScore += peakScore;

        // Util::log("checkFit: peakScore %8.2f (total %8.2f)", peakScore, totalScore);
    }

    // float avgDist    = 1.0 * sumDist    / libraryPeaks.size();
    // float avgDistPos = 1.0 * sumDistPos / libraryPeaks.size();
    // Util::log("checkFit: sumDist %8d avgDist %8.2f sumDistPos %8d avgDistPos %8.2f", sumDist, avgDist, sumDistPos, avgDistPos);

    // Util::log("checkFit: returning totalScore %.2f", totalScore);
    return totalScore;
}

// we're defining a peak as one which is the highest for at least MIN_RAMP_PIXELS
// in either direction, as well as MIN_PEAK_HEIGHT counts above the left-hand
// ramp shoulder.
vector<int> Identify::Interim::findPeakPixels(const vector<float>& spectrum, int minRampPixels, int minPeakHeight) const
{
    vector<int> peakPixels;
    int rampLeft = 0;
    float rampBase = spectrum[0];
    for (int i = 1; i < (int)spectrum.size() - minRampPixels; i++)
    {
        // is ramp increasing?
        if (spectrum[i] > spectrum[i-1])
        {
            rampLeft++;
            if (rampLeft >= minRampPixels)
            {
                // we satisfied the left-ramp, now see if the right-ramp 
                // indicates we're at a local maxima
                bool maxima = true;
                for (int j = i; j < i + minRampPixels && maxima; j++)
                    if (spectrum[j] <= spectrum[j+1])
                        maxima = false;
                if (maxima)
                {
                    // apparently this is a local maxima

                    // is it HIGH ENOUGH?
                    if (spectrum[i] >= rampBase + minPeakHeight)
                        peakPixels.push_back(i);
                }
            }
        }
        else
        {
            rampLeft = 0;
            rampBase = spectrum[0];
        }
    }
    return peakPixels;
}

vector<float> Identify::Interim::boxcar(const vector<float>& spectrum, int halfWidth) const
{
	vector<float> smoothed(spectrum.size());
	for (int i = 0; i < (int)spectrum.size(); i++)
    {
        if (i < halfWidth || i + halfWidth >= (int)spectrum.size())
            smoothed[i] = spectrum[i];
        else
        {
            float sum = 0;
            for (int j = i - halfWidth; j <= i + halfWidth; j++)
                sum += spectrum[j];
            smoothed[i] = sum / (halfWidth * 2 + 1);
        }
    }
    return smoothed;
}
