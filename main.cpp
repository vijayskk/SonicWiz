#include "audio/audiomanager.h"
#include <string>
#include <cstring>

static streamCallbackData *spectroData;

int main(int argc, char const *argv[])
{
    initializePortAudio();

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

    int selectedDevice = getDevice();

    PaStream * stream = createStream(selectedDevice,spectroData);

    

    while (true)
    {
        // Game Loop
    }

    destroyStream(stream);

    fftw_destroy_plan(spectroData->p);
    fftw_free(spectroData->in);
    fftw_free(spectroData->out);
    free(spectroData);

    printf("\n");
    return EXIT_SUCCESS;
}
