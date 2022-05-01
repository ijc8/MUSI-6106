#include <juce_audio_formats/juce_audio_formats.h>

#include "CommentarySonifier.h"
#include "Waveform.h"
#include "ChessSoundData.h"

CommentarySonifier::CommentarySonifier(float sampleRate) : Sonifier(sampleRate) {
    // Example: here's how to play an audio file from a sonifier.
    // Feel free to build on this for e.g. a stem-mixing sonifier.
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    int size;
    const char *memory = ChessSoundData::getNamedResource("queen_ogg", size);
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::make_unique<juce::MemoryInputStream>(memory, size, false)));
    buffer.setSize(reader->numChannels, reader->lengthInSamples, false, false, false);
    reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);

    double rate = 1.0 / ((double)reader->lengthInSamples / reader->sampleRate);
    std::shared_ptr<CWavetableOscillator> osc = std::make_shared<CWavetableOscillator>(buffer, rate, 1.0, sampleRate);
    mMainProcessor.addInst(osc);
    osc->noteOn();
}

void CommentarySonifier::onMove(Chess::Game &board) {
    (void)board;
}
