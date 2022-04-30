#ifndef COMMENTARY_SONIFIER_H
#define COMMENTARY_SONIFIER_H

#include "Sonifier.h"

class CommentarySonifier: public Sonifier {
  public:
    CommentarySonifier(float sampleRate);
  protected:
    void onMove(Chess::Game &board) override;

  private:
    juce::AudioSampleBuffer buffer;
};

#endif // COMMENTARY_SONIFIER_H
