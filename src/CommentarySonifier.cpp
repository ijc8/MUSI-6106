#include <juce_audio_formats/juce_audio_formats.h>

#include "CommentarySonifier.h"
#include "Waveform.h"

CommentarySonifier::CommentarySonifier() {
}

void CommentarySonifier::prepareToPlay(int expectedBlockSize, float sampleRate) {
    (void)expectedBlockSize;
    (void)sampleRate;

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    juce::File file("/home/ian/GT/MUSI-6106/MUSI-6106/test.wav");
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
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
