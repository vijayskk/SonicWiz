#include <stdio.h>
#include <cmath>
#include <bits/stdc++.h>

void visualize(int displaySize,const double samplingRate,int spectroWidth,double * out){
    const double nyquist = samplingRate / 2.0;
    const double f_min = 20.0;    // lowest freq to show
    const double f_max = nyquist; // highest freq to show (or less)

    // Precompute log ratio
    const double logRatio = std::log(f_max / f_min);

    // Loop over display columns
    for (int i = 0; i < displaySize; ++i)
    {
        // Normalized center of this column
        double propCenter = static_cast<double>(i) / (displaySize - 1);

        // Compute log-spaced center frequency
        double f_center = f_min * std::exp(logRatio * propCenter);

        // Compute log-spaced half-width of this column in frequency
        // (one column width in prop = 1/displaySize)
        double propHalf = 0.5 / displaySize;
        double f_low = f_min * std::exp(logRatio * (propCenter - propHalf));
        double f_high = f_min * std::exp(logRatio * (propCenter + propHalf));

        // Clamp
        if (f_low < f_min)
            f_low = f_min;
        if (f_high > f_max)
            f_high = f_max;

        // Map frequencies to FFT bin floats
        double binLowF = (f_low / nyquist) * spectroWidth;
        double binHighF = (f_high / nyquist) * spectroWidth;

        // Integer parts for loop
        int binLow = static_cast<int>(std::floor(binLowF));
        int binHigh = static_cast<int>(std::floor(binHighF));

        // Clamp bin indices to safe range
        binLow = std::clamp(binLow, 0, spectroWidth - 1);
        binHigh = std::clamp(binHigh, binLow, spectroWidth - 1);

        double sum = 0.0;
        double weightSum = 0.0;

        // --- average + interpolate at edges ---
        for (int b = binLow; b <= binHigh; ++b)
        {
            double w = 1.0; // weight (default full bin)
            if (b == binLow)
            {
                // fractional coverage of the first bin
                w = 1.0 - (binLowF - std::floor(binLowF));
            }
            else if (b == binHigh)
            {
                // fractional coverage of the last bin
                w = (binHighF - std::floor(binHighF));
            }

            sum += out[b] * w;
            weightSum += w;
        }

        double value = (weightSum > 0.0) ? (sum / weightSum) : 0.0;

        // Map value to visual block
        if (value < 0.125)
            std::printf("▁");
        else if (value < 0.25)
            std::printf("▂");
        else if (value < 0.375)
            std::printf("▃");
        else if (value < 0.5)
            std::printf("▄");
        else if (value < 0.625)
            std::printf("▅");
        else if (value < 0.75)
            std::printf("▆");
        else if (value < 0.875)
            std::printf("▇");
        else
            std::printf("█");
    }

    fflush(stdout);
}