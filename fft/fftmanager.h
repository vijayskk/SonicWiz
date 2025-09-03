#include "../include/streamcallback.h"
#include "../include/helper.h"
void initializeFFT(streamCallbackData * sd){
    sd->in = (double *)malloc(sizeof(double) * FRAMES_PER_BUFFER);
    sd->out = (double *)malloc(sizeof(double) * FRAMES_PER_BUFFER);
    if (sd->in == NULL || sd->out == NULL)
    {
        printf("ERROR: Memory Allocation Failed!!\n");
        exit(EXIT_FAILURE);
    }
    sd->p = fftw_plan_r2r_1d(FRAMES_PER_BUFFER, sd->in, sd->out, FFTW_R2HC, FFTW_ESTIMATE);
    double sampleRatio = FRAMES_PER_BUFFER / SAMPLE_RATE;
    sd->startIndex = std::ceil(sampleRatio * SPECTRO_FREQ_START);
    sd->spectroWidth = min(std::ceil(sampleRatio * SPECTRO_FREQ_END), FRAMES_PER_BUFFER / 2.0) - sd->startIndex;
}