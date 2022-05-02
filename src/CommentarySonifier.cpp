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

    std::pair<Chess::Color, std::string> colorNames[] = {{Chess::Color::White, "white"}, {Chess::Color::Black, "black"}};
    for (auto &[color, name] : colorNames) {
        std::string resourceName = name + "_ogg";
        juce::AudioSampleBuffer &buffer = *colors.emplace(color, std::make_unique<juce::AudioSampleBuffer>()).first->second;
        audioSampleRate = loadSound(formatManager, resourceName.c_str(), buffer);
    }

    for (auto &[type, typeName] : Chess::Piece::ToChar) {
        juce::AudioSampleBuffer &buffer = *pieces.emplace(type, std::make_unique<juce::AudioSampleBuffer>()).first->second;
        std::string name = std::string({typeName}) + "_ogg";
        audioSampleRate = loadSound(formatManager, name.c_str(), buffer);
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
    if (game.peek()) {
        auto [move, state] = game.getHistory().top();
        Chess::Piece::Type piece = move.promotion ? Chess::Piece::Type::Pawn : game.getPieceAt(move.dst)->type;
        std::cout << "Last move: " << move.toString() << std::endl;
        std::vector<juce::AudioSampleBuffer *> buffers;
        // TODO: Handle castling.
        if (piece != Chess::Piece::Type::Pawn) {
            buffers.push_back(pieces[piece].get());
        }
        buffers.push_back(squares[move.src].get());
        if (state.getPieceAt(move.dst)) {
            // A piece was captured.
            buffers.push_back(&takes);
        }
        buffers.push_back(squares[move.dst].get());
        if (move.promotion) {
            buffers.push_back(&equals);
            buffers.push_back(pieces[*move.promotion].get());
        }
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
        double start = 0;
        for (auto buffer : buffers) {
            double duration = (double)buffer->getNumSamples() / audioSampleRate;
            double rate = 1.0 / duration;
            mMainProcessor.scheduleInst(std::make_unique<CWavetableOscillator>(*buffer, rate, 1.0, mSampleRate), start, duration);
            start += duration;
        }
    }
}
