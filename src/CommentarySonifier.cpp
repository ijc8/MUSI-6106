#include <juce_audio_formats/juce_audio_formats.h>

#include "CommentarySonifier.h"
#include "Waveform.h"
#include "ChessSoundData.h"

double loadSound(juce::AudioFormatManager &formatManager, const char *name, juce::AudioSampleBuffer &buffer) {
    int size;
    const char *memory = ChessSoundData::getNamedResource(name, size);
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::make_unique<juce::MemoryInputStream>(memory, size, false)));
    buffer.setSize(reader->numChannels, reader->lengthInSamples, false, false, false);
    reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);
    return reader->sampleRate;
}

CommentarySonifier::CommentarySonifier(float sampleRate) : Sonifier(sampleRate) {
    // Load bundled audio clips.
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::pair<Chess::Color, std::string> colorNames[] = {{Chess::Color::White, "white"}, {Chess::Color::Black, "black"}};
    for (auto &[color, name] : colorNames) {
        std::string resourceName = name + "_ogg";
        juce::AudioSampleBuffer &buffer = *colors.emplace(color, std::make_unique<juce::AudioSampleBuffer>()).first->second;
        audioSampleRate = loadSound(formatManager, resourceName.c_str(), buffer);
    }

    for (auto &[type, name] : Chess::Piece::ToChar) {
        juce::AudioSampleBuffer &buffer = *pieces.emplace(type, std::make_unique<juce::AudioSampleBuffer>()).first->second;
        std::string resourceName = std::string({name}) + "_ogg";
        audioSampleRate = loadSound(formatManager, resourceName.c_str(), buffer);
    }

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            Chess::Square square(rank, file);
            juce::AudioSampleBuffer &buffer = *squares.emplace(square, std::make_unique<juce::AudioSampleBuffer>()).first->second;
            std::string name = square.toString() + "_ogg";
            audioSampleRate = loadSound(formatManager, name.c_str(), buffer);
        }
    }

    std::pair<std::string, juce::AudioSampleBuffer *> misc[] = {
        {"equals", &equals},
        {"takes", &takes},
        {"wins", &wins},
        {"check", &check},
        {"checkmate", &checkmate},
        {"stalemate", &stalemate},
        {"OO", &castleShort},
        {"OOO", &castleLong},
    };
    for (auto &[name, buffer] : misc) {
        std::string resourceName = name + "_ogg";
        audioSampleRate = loadSound(formatManager, resourceName.c_str(), *buffer);
    }
}

void CommentarySonifier::onMove(Chess::Game &game) {
    if (!game.peek()) return;

    // Announce last move by concatenating recorded clips.
    auto [move, state] = game.getHistory().top();
    Chess::Piece::Type piece = move.promotion ? Chess::Piece::Type::Pawn : game.getPieceAt(move.dst)->type;
    std::vector<juce::AudioSampleBuffer *> buffers;
    // Did we castle?
    if (piece == Chess::Piece::Type::King && abs(move.src.file - move.dst.file) > 1) {
        // Yes: announce the kind of castling.
        buffers.push_back(move.dst.file < 4 ? &castleLong : &castleShort);
    } else {
        // No: announce piece, start square, and destination square.
        // (This is the long algebraic notation.)
        if (piece != Chess::Piece::Type::Pawn) {
            buffers.push_back(pieces[piece].get());
        }
        buffers.push_back(squares[move.src].get());
        if (state.getPieceAt(move.dst)) {
            // Announce capture.
            buffers.push_back(&takes);
        }
        buffers.push_back(squares[move.dst].get());
    }
    // Announce promotion.
    if (move.promotion) {
        buffers.push_back(&equals);
        buffers.push_back(pieces[*move.promotion].get());
    }
    // Announce outcome.
    auto outcome = game.getOutcome();
    if (outcome) {
        auto winner = *outcome;
        if (winner) {
            buffers.push_back(&checkmate);
            buffers.push_back(colors[*winner].get());
            buffers.push_back(&wins);
        } else {
            buffers.push_back(&stalemate);
        }
    } else if (game.isInCheck(game.getTurn())) {
        buffers.push_back(&check);
    }
    // Play clips in sequence.
    double start = 0;
    for (auto buffer : buffers) {
        double duration = (double)buffer->getNumSamples() / audioSampleRate;
        double rate = 1.0 / duration;
        mMainProcessor.scheduleInst(std::make_unique<CWavetableOscillator>(*buffer, rate, 1.0, mSampleRate), start, duration);
        start += duration;
    }
}
