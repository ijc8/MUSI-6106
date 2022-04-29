#ifndef COMMENTARY_SONIFIER_H
#define COMMENTARY_SONIFIER_H

#include "Sonifier.h"

class CommentarySonifier: public Sonifier {
  public:
    CommentarySonifier();
    void prepareToPlay(int expectedBlockSize, float sampleRate) override;
  protected:
    void onMove(Chess::Game &board) override;

  private:
    juce::AudioSampleBuffer buffer;
};

#endif // COMMENTARY_SONIFIER_H
