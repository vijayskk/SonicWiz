#include <cstring>
#include "../include/streamcallback.h"


void initializePortAudio(){
    PaError error;
    error = Pa_Initialize();
    checkError(error);
}

int getDevice(){
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
    return selectedDevice;
}

PaStream * createStream(int selectedDevice,streamCallbackData * spectroData){
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
    PaError error = Pa_OpenStream(
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
    return stream;
}

void destroyStream(PaStream * stream){
    PaError error = Pa_StopStream(stream);
    checkError(error);

    error = Pa_CloseStream(stream);
    checkError(error);

    error = Pa_Terminate();
    checkError(error);
}