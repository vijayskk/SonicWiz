#include "defenitions.h"
#include <portaudio.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <string>


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
