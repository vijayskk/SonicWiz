#include "defenitions.h"
#include "helper.h"
#include <fftw3.h>
#include <portaudio.h>
#include "terminalvisualizer.h"

typedef struct streamCallbackData
{
    double *in;
    double *out;
    fftw_plan p;
    int startIndex;
    int spectroWidth;
} streamCallbackData;

static int streamCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userdata)
{
    float *in = (float *)inputBuffer;
    (void)outputBuffer;
    streamCallbackData *callBackData = (streamCallbackData *)userdata;

    int displaySize = 100;
    printf("\r");

    for (unsigned long i = 0; i < framesPerBuffer; i++)
    {
        callBackData->in[i] = in[i * NUM_CHANNELS];
    }

    fftw_execute(callBackData->p);

    visualize(displaySize,SAMPLE_RATE,callBackData->spectroWidth,callBackData->out);
    
    return 0;
}