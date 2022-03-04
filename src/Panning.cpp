//
// Created by Rose Sun on 2/28/22.
//

#include "Panning.h"
// stereo to stereo version pan
#include <math.h>

Panning::Panning(float fPan) {
    m_fPan = fPan;
}

Panning::~Panning() {

}

Error_t Panning::process(float **ppfIn, float **ppfOut, int iNumberOfFrames)
{
    for (int i = 0; i < iNumberOfFrames; i++) {
        if (m_fPan > 0) {
            ppfOut[0][i] = (1 - m_fPan) * ppfIn[0][i];
            ppfOut[1][i] = ppfIn[1][i];
        }
        else if (m_fPan < 0) {
            ppfOut[0][i] = ppfIn[0][i];
            ppfOut[1][i] = (1 + m_fPan) * ppfIn[1][i];
        }
        else {
            ppfOut = ppfIn;
        }
    }
    return kNoError;
}