#include "CommentarySonifier.h"
#include "Waveform.h"

CommentarySonifier::CommentarySonifier() {
}

void CommentarySonifier::prepareToPlay(int expectedBlockSize, float sampleRate) {
    std::shared_ptr<CWavetableOscillator> osc = std::make_shared<CWavetableOscillator>(Waveform::sine, 400.0, 1.0, sampleRate);
    mMainProcessor.addInst(osc);
    osc->noteOn();
}

void CommentarySonifier::onMove(Chess::Game &board) {
}
