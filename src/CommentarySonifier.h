#ifndef COMMENTARY_SONIFIER_H
#define COMMENTARY_SONIFIER_H

#include "Sonifier.h"

class CommentarySonifier: public Sonifier {
public:
    CommentarySonifier(float sampleRate);

protected:
    void onMove(Chess::Game &game) override;

private:
    double audioSampleRate;
    std::unordered_map<Chess::Color, std::unique_ptr<juce::AudioSampleBuffer>> colors;
    std::unordered_map<Chess::Piece::Type, std::unique_ptr<juce::AudioSampleBuffer>> pieces;
    std::unordered_map<Chess::Square, std::unique_ptr<juce::AudioSampleBuffer>> squares;
    juce::AudioSampleBuffer equals, takes, wins;
    juce::AudioSampleBuffer check, checkmate, stalemate;
    juce::AudioSampleBuffer castleShort, castleLong;
};

#endif // COMMENTARY_SONIFIER_H
