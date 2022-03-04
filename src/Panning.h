//
// Created by Rose Sun on 2/28/22.
//

#ifndef MUSICAL_CHESS_PANNING_H
#define MUSICAL_CHESS_PANNING_H

#include "ErrorDef.h"

class Panning{
public:
    Panning(float fPan);

    virtual ~Panning();

    Error_t process(float **ppfIn, float **ppfOut, int iNumberOfFrames);

protected:
    float m_fPan;
};

#endif //MUSICAL_CHESS_PANNING_H
