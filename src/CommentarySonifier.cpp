#include <juce_audio_formats/juce_audio_formats.h>

#include "CommentarySonifier.h"
#include "Waveform.h"
#include "ChessSoundData.h"

double loadSound(juce::AudioFormatManager &formatManager, const char *name, juce::AudioSampleBuffer &buffer) {
    std::cout << "Loading " << name << std::endl;
    int size;
    const char *memory = ChessSoundData::getNamedResource(name, size);
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::make_unique<juce::MemoryInputStream>(memory, size, false)));
    buffer.setSize(reader->numChannels, reader->lengthInSamples, false, false, false);
    reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);
    return reader->sampleRate;
}

CommentarySonifier::CommentarySonifier(float sampleRate) : Sonifier(sampleRate) {
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    for (int rank = 0; rank < 8; rank++) {
        std::string name = "_" + std::to_string(rank + 1) + "_ogg";
        audioSampleRate = loadSound(formatManager, name.c_str(), ranks[rank]);
    }

    for (int file = 0; file < 8; file++) {
        std::string name = std::string({(char)(file + 'a')}) + "_ogg";
        audioSampleRate = loadSound(formatManager, name.c_str(), files[file]);
    }

    for (auto &[type, typeName] : Chess::Piece::ToChar) {
        juce::AudioSampleBuffer &buffer = *pieces.emplace(type, std::make_unique<juce::AudioSampleBuffer>()).first->second;
        std::string name = std::string({typeName}) + "_ogg";
        audioSampleRate = loadSound(formatManager, name.c_str(), buffer);
    }
}

void CommentarySonifier::onMove(Chess::Game &board) {
    (void)board;
    std::optional<Chess::Move> last = board.peek();
    if (last) {
        Chess::Piece::Type piece = board.getPieceAt(last->dst)->type;
        std::cout << "Last move: " << last->toString() << std::endl;
        juce::AudioSampleBuffer *buffers[] = {pieces[piece].get(), &files[last->src.file], &ranks[last->src.rank]};
        double start = 0;
        for (int i = 0; i < 3; i++) {
            juce::AudioSampleBuffer &buffer = *buffers[i];
            double duration = (double)buffer.getNumSamples() / audioSampleRate;
            double rate = 1.0 / duration;
            mMainProcessor.scheduleInst(std::make_unique<CWavetableOscillator>(buffer, rate, 1.0, mSampleRate), start, duration);
            start += duration;
        }
    }
}
