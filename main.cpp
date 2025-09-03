#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>
#include <string>
#include <cstring>
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

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

static inline float absf(float a)
{
    return a > 0 ? a : -a;
}

static int paTestCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userdata)
{
    float *in = (float *)inputBuffer;
    (void)outputBuffer;

    int displaySize = 100;
    printf("\r");

    float vol_l = 0;
    float vol_r = 0;

    for (unsigned long i = 0; i < framesPerBuffer; i += 2)
    {
        vol_l = max(vol_l, absf(in[i]));
        vol_r = max(vol_r, absf(in[i + 1]));
    }

    for (int i = 0; i < displaySize; i++)
    {
        float bar = i / (float)displaySize;
        if (bar <= vol_l && bar <= vol_r)
        {
            printf("█");
        }
        else if (bar <= vol_l)
        {
            printf("▀");
        }
        else if (bar <= vol_r)
        {
            printf("▄");
        }
        else
        {
            printf(" ");
        }
    }

    fflush(stdout);
    return 0;
}

int main(int argc, char const *argv[])
{
    PaError error;
    error = Pa_Initialize();
    checkError(error);

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

    inputParameters.channelCount = 2;
    inputParameters.device = selectedDevice;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(selectedDevice)->defaultLowInputLatency;

    outputParameters.channelCount = 2;
    outputParameters.device = selectedDevice;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(selectedDevice)->defaultLowInputLatency;

    PaStream *stream;
    error = Pa_OpenStream(
        &stream,
        &inputParameters,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        paTestCallback,
        NULL);
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
    return EXIT_SUCCESS;
}
