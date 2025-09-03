#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>
#include <string>
#include <cstring>
#include <fftw3.h>
#include <cmath>
#include <bits/stdc++.h>

#define SAMPLE_RATE 44100.0
#define FRAMES_PER_BUFFER 512
#define NUM_CHANNELS 2
#define SPECTRO_FREQ_START 20
#define SPECTRO_FREQ_END 20000

typedef struct
{
    double *in;
    double *out;
    fftw_plan p;
    int startIndex;
    int spectroWidth;
} streamCallbackData;

static streamCallbackData *spectroData;

static void checkError(PaError err)
{
    if (err != paNoError)
    {
        printf("PORTAUDIO ERROR: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

static inline float max(float a, float b)
{
    return a > b ? a : b;
}

static inline float min(float a, float b)
{
    return a < b ? a : b;
}

static inline float absf(float a)
{
    return a > 0 ? a : -a;
}

static int streamCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userdata)
{
    float *in = (float *)inputBuffer;
    (void)outputBuffer;
    streamCallbackData *callBackData = (streamCallbackData *)userdata;

    int displaySize = 100;
    printf("\r");

    // float vol_l = 0;
    // float vol_r = 0;

    // for (unsigned long i = 0; i < framesPerBuffer; i += 2)
    // {
    //     vol_l = max(vol_l, absf(in[i]));
    //     vol_r = max(vol_r, absf(in[i + 1]));
    // }

    // for (int i = 0; i < displaySize; i++)
    // {
    //     float bar = i / (float)displaySize;
    //     if (bar <= vol_l && bar <= vol_r)
    //     {
    //         printf("█");
    //     }
    //     else if (bar <= vol_l)
    //     {
    //         printf("▀");
    //     }
    //     else if (bar <= vol_r)
    //     {
    //         printf("▄");
    //     }
    //     else
    //     {
    //         printf(" ");
    //     }
    // }

    for (unsigned long i = 0; i < framesPerBuffer; i++)
    {
        callBackData->in[i] = in[i * NUM_CHANNELS];
    }

    fftw_execute(callBackData->p);

    const double samplingRate = SAMPLE_RATE; // for example
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
        double binLowF = (f_low / nyquist) * callBackData->spectroWidth;
        double binHighF = (f_high / nyquist) * callBackData->spectroWidth;

        // Integer parts for loop
        int binLow = static_cast<int>(std::floor(binLowF));
        int binHigh = static_cast<int>(std::floor(binHighF));

        // Clamp bin indices to safe range
        binLow = std::clamp(binLow, 0, callBackData->spectroWidth - 1);
        binHigh = std::clamp(binHigh, binLow, callBackData->spectroWidth - 1);

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

            sum += callBackData->out[b] * w;
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
    return 0;
}

int main(int argc, char const *argv[])
{
    PaError error;
    error = Pa_Initialize();
    checkError(error);

    spectroData = (streamCallbackData *)malloc(sizeof(streamCallbackData));
    spectroData->in = (double *)malloc(sizeof(double) * FRAMES_PER_BUFFER);
    spectroData->out = (double *)malloc(sizeof(double) * FRAMES_PER_BUFFER);
    if (spectroData->in == NULL || spectroData->out == NULL)
    {
        printf("ERROR: Memory Allocation Failed!!\n");
        exit(EXIT_FAILURE);
    }
    spectroData->p = fftw_plan_r2r_1d(FRAMES_PER_BUFFER, spectroData->in, spectroData->out, FFTW_R2HC, FFTW_ESTIMATE);
    double sampleRatio = FRAMES_PER_BUFFER / SAMPLE_RATE;
    spectroData->startIndex = std::ceil(sampleRatio * SPECTRO_FREQ_START);
    spectroData->spectroWidth = min(std::ceil(sampleRatio * SPECTRO_FREQ_END), FRAMES_PER_BUFFER / 2.0) - spectroData->startIndex;

    int numDevices = Pa_GetDeviceCount();
    printf("Number of Devices : %d\n\n", numDevices);
    if (numDevices < 0)
    {
        printf("ERROR: Device count query failed.");
        exit(EXIT_FAILURE);
    }
    else if (numDevices == 0)
    {
        printf("ERROR: No devices found.");
        exit(EXIT_FAILURE);
    }

    PaDeviceInfo *deviceInfo;
    int selectedDevice = 0;
    for (int i = 0; i < numDevices; i++)
    {
        deviceInfo = (PaDeviceInfo *)Pa_GetDeviceInfo(i);
        printf("Device: %d\n", i);
        printf("\tName: %s\n", deviceInfo->name);
        printf("\tMax Input Channels: %d\n", deviceInfo->maxInputChannels);
        printf("\tMax Output Channels: %d\n", deviceInfo->maxOutputChannels);
        printf("\tSample Rate: %f\n\n", deviceInfo->defaultSampleRate);
        if (std::string(deviceInfo->name) == std::string("pulse"))
        {
            selectedDevice = i;
            printf("Selected: %d", selectedDevice);
        }
    }

    PaStreamParameters inputParameters;
    PaStreamParameters outputParameters;
    memset(&inputParameters, 0, sizeof(inputParameters));
    memset(&outputParameters, 0, sizeof(outputParameters));

    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.device = selectedDevice;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(selectedDevice)->defaultLowInputLatency;

    outputParameters.channelCount = NUM_CHANNELS;
    outputParameters.device = selectedDevice;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(selectedDevice)->defaultLowInputLatency;

    PaStream *stream;
    error = Pa_OpenStream(
        &stream,
        &inputParameters,
        NULL,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        streamCallback,
        spectroData);
    checkError(error);

    error = Pa_StartStream(stream);
    checkError(error);

    while (true)
    {
        /* code */
    }

    error = Pa_StopStream(stream);
    checkError(error);

    error = Pa_CloseStream(stream);
    checkError(error);

    error = Pa_Terminate();
    checkError(error);

    fftw_destroy_plan(spectroData->p);
    fftw_free(spectroData->in);
    fftw_free(spectroData->out);
    free(spectroData);

    printf("\n");
    return EXIT_SUCCESS;
}
